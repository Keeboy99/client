#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>
#include <utils/memory.hpp>
#include <game/game.hpp>

#include "component/filesystem.hpp"

using json = nlohmann::json;

namespace omnimix
{
	utils::hook::detour get_name_hook;
	const char* get_bga_name(game::music_t* music, int note_id)
	{
		const auto music_bga = music->bga_filename;
		auto path = utils::string::va("/data/movie/%s.mp4", music_bga);

		if (filesystem::exists(path)) {
			return utils::string::va("%s.mp4", music_bga);
		}

		path = utils::string::va("/data/movie/%s.wmv", music_bga);

		if (filesystem::exists(path)) {
			return utils::string::va("%s.wmv", music_bga);
		}

		return get_name_hook.invoke<const char*, game::music_t*, int>(music, note_id);
	}

	utils::hook::detour load_music_info_hook;
	uint64_t load_music_info()
	{
		auto result = load_music_info_hook.invoke<uint64_t>();
		const auto music_data = game::get_music_data();

		filesystem::file omni_file{ "/data/omni_data.json" };

		if (!omni_file.exists()) {
			printf("E:omnimix: can not load /data/omni_data.json\n");
			return result;
		}

		simdjson::dom::parser parser;
		simdjson::padded_string padded{ omni_file.get_buffer() };
		auto omni_data = parser.parse(padded);

		for (const auto& item : omni_data)
		{
			if (!item.is_object()) {
				printf("W:omnimix: music item is not object!\n");
				continue;
			}

			const size_t id = item["id"].get_uint64();
			const auto index = music_data->index_table[id];

			if ((id >= CUR_STYLE_ENTRIES && index == 0) || index == 0xffff)
				continue;

			auto* const music = music_data->musics + index;

			for (size_t i = 0; i < 5; i++)
			{
				music->bpm[i].min = static_cast<uint32_t>(item["bpms"]["sp"].at(i)["min"].get_uint64());
				music->bpm[i].max = static_cast<uint32_t>(item["bpms"]["sp"].at(i)["max"].get_uint64());
				music->bpm[i + 5].min = static_cast<uint32_t>(item["bpms"]["dp"].at(i)["min"].get_uint64());
				music->bpm[i + 5].max = static_cast<uint32_t>(item["bpms"]["dp"].at(i)["max"].get_uint64());

				music->note_count[i] = static_cast<uint32_t>(item["noteCounts"]["sp"].at(i).get_uint64());
				music->note_count[i + 5] = static_cast<uint32_t>(item["noteCounts"]["dp"].at(i).get_uint64());
			}
		}

		return result;
	}

	void insert_music_datas()
	{
		filesystem::file omni_file{ "/data/omni_data.json" };

		if (!omni_file.exists()) {
			printf("E:omnimix: can not load /data/omni_data.json\n");
			return game::finalize_music_data();
		}

		const auto music_data = game::get_music_data();
		auto backup = utils::memory::allocate<game::music_data_t>(0x400000);
		std::memcpy(backup, music_data, 0x400000);

		auto _ = gsl::finally([=] {
			game::finalize_music_data();
			utils::memory::free(backup);
		});

		simdjson::dom::parser parser;
		simdjson::padded_string padded{ omni_file.get_buffer() };
		auto omni_data = parser.parse(padded);
		std::vector<game::music_t> musics;

		if (!omni_data.is_array()) {
			printf("E:omnimix: omni_data is not array!\n");
			return;
		}

		for (const auto& item : omni_data)
		{
			if (!item.is_object()) {
				printf("W:omnimix: music item is not object!\n");
				continue;
			}

			game::music_t music = { 0 };

#define LOAD_STRING(field, path) \
	{ \
		const std::string str(item##path.get_string().value()); \
		const auto shiftjis_str = utils::string::wide_to_shiftjis(utils::string::utf8_to_wide(str)); \
		memcpy(music.##field, shiftjis_str.data(), std::min(sizeof(music.##field), shiftjis_str.size())); \
	}

#define LOAD_INT(field, path) \
	music.##field = static_cast<uint32_t>(item##path.get_int64());
#define LOAD_SHORT(field, path) \
	music.##field = static_cast<uint16_t>(item##path.get_int64());
#define LOAD_BYTE(field, path) \
	music.##field = static_cast<uint8_t>(item##path.get_int64());

			LOAD_INT(song_id, ["id"]);

			LOAD_STRING(title, ["title"]);
			LOAD_STRING(title_ascii, ["englishTitle"]);
			LOAD_STRING(genre, ["genre"]);
			LOAD_STRING(artist, ["artist"]);

			LOAD_INT(texture_title, ["textures"]["title"]);
			LOAD_INT(texture_artist, ["textures"]["artist"]);
			LOAD_INT(texture_genre, ["textures"]["genre"]);
			LOAD_INT(texture_load, ["textures"]["load"]);
			LOAD_INT(texture_list, ["textures"]["list"]);

			LOAD_INT(font_idx, ["fontId"]);
			LOAD_SHORT(game_version, ["version"]);

			LOAD_SHORT(other_folder, ["otherFolder"]);
			LOAD_SHORT(bemani_folder, ["bemaniFolder"]);
			LOAD_SHORT(splittable_diff, ["splitTableDiff"]);

			for (int i = 0; i < 5; i++)
			{
				LOAD_BYTE(level[i], ["level"]["sp"].at(i));
				LOAD_BYTE(level[i + 5], ["level"]["dp"].at(i));
				LOAD_BYTE(ident_sp[i], ["ident"]["sp"].at(i));
				LOAD_BYTE(ident_dp[i], ["ident"]["dp"].at(i));
			}

			LOAD_INT(song_id, ["id"]);
			LOAD_INT(volume, ["volume"]);

			LOAD_SHORT(bga_delay, ["bgaDelay"]);
			LOAD_STRING(bga_filename, ["bgaFile"]);

			LOAD_INT(afp_flag, ["afpFlag"]);

			if (!item["afpData"].error() == 0)
			{
				musics.push_back(music);
				continue;
			}

			if (!item["afpData"].is_array())
			{
				printf("W:omnimix: afp_data is not array.\n");
				continue;
			}

			for (size_t n = 0; n < 10; n++)
			{
				std::string hex(item["afpData"].at(n).get_string().value());

				for (size_t i = 0; i < 0x20; i++)
				{
					const auto& byte = hex.substr(i * 2, 2);
					music.afp_data[n * 0x20 + i] = static_cast<uint8_t>(std::strtoul(byte.data(), nullptr, 16));
				}
			}

			musics.push_back(music);
		}

		printf("I:omnimix: parsed %llu musics\n", musics.size());

		for (size_t i = 0; i < backup->music_count; i++)
			musics.push_back(backup->musics[i]);

		printf("I:omnimix: total music count: %llu\n", musics.size());

		std::sort(musics.begin(), musics.end(), [](const game::music_t& a, const game::music_t& b) -> bool {
			return a.song_id < b.song_id;
		});

		music_data->music_count = static_cast<uint16_t>(musics.size());

		for (size_t i = 0; i < MAX_ENTRIES; i++)
		{
			const auto find_result = std::find_if(musics.begin(), musics.end(), [i](const game::music_t& v) {
				return v.song_id == i;
				});

			if (find_result != musics.end())
			{
				music_data->index_table[i] = static_cast<uint16_t>(find_result - musics.begin());
				continue;
			}

			if (i >= CUR_STYLE_ENTRIES)
				music_data->index_table[i] = 0;
			else
				music_data->index_table[i] = 0xffffi16;
		}

		for (size_t i = 0; i < music_data->music_count; i++)
			std::memcpy(music_data->musics + i, &musics[i], sizeof(game::music_t));
	}

	class component final : public component_interface
	{
	public:
		void post_start() override
		{
			// return success if file not exists
			utils::hook::set<uint16_t>(0x1401E49E8, 0x01B0);

			// allow mp4 and wmv bga
			get_name_hook.create(0x1401B7910, get_bga_name);

			// add omni songs to music_data.bin
			utils::hook::call(0x1401B675E, insert_music_datas);

			// load omni song detail
			load_music_info_hook.create(0x1401B67A0, load_music_info);
		}
	};
}

REGISTER_COMPONENT(omnimix::component)