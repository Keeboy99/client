<script setup lang="ts">
import { iidx } from '@/modules/iidx';
import { sdvx } from '@/modules/sdvx';
import { gitadora } from '@/modules/gitadora';
import { VersionState } from "@/modules/launcher";
</script>

<template>
  <main>
    <div></div>
    <div class="game sdvx" :class="{'disable': sdvx.GameMeta.value == undefined}">
      <div class="background"></div>
      <div class="text">
        SOUND VOLTEX<br>
        EXCEED GEAR
        <div v-if="sdvx.GameMeta.value != undefined && !sdvx.installed()" class="tip">
          Not Installed
        </div>
      </div>
      <div v-if="sdvx.GameMeta.value != undefined && sdvx.installed()" class="options">
        <div v-if="sdvx.GameVersionState.value == VersionState.Normal" class="opt" @click="sdvx.start">Start</div>
        <div v-if="sdvx.GameVersionState.value == VersionState.Need2UpdateLauncher" class="tip opt">
          Please Update Launcher
        </div>
        <div v-if="sdvx.GameVersionState.value == VersionState.Need2UpdateGame" class="tip opt">
          Please Update Game
        </div>
        <div class="opt" @click="sdvx.settings">Game Settings</div>
        <div v-if="sdvx.GameVersionState.value != VersionState.Need2UpdateGame" class="opt" @click="sdvx.updater">Updater
        </div>
        <div v-else class="blink opt" @click="sdvx.updater">Updater</div>
      </div>
    </div>
    <div class="game iidx" :class="{'disable': iidx.GameMeta.value == undefined}">
      <div class="background"></div>
      <div class="text">
        beatmania IIDX<br>
        INFINITAS
        <div v-if="iidx.GameMeta.value != undefined && !iidx.installed()" class="tip">
          Not Installed
        </div>
      </div>
      <div v-if="iidx.GameMeta.value != undefined && iidx.installed()" class="options">
        <div v-if="iidx.GameVersionState.value == VersionState.Normal" class="opt" @click="iidx.start">Start</div>
        <div v-if="iidx.GameVersionState.value == VersionState.Need2UpdateLauncher" class="tip opt">
          Please Update Launcher
        </div>
        <div v-if="iidx.GameVersionState.value == VersionState.Need2UpdateGame" class="tip opt">
          Please Update Game
        </div>
        <RouterLink class="opt" to="/iidx/settings">Extra Settings</RouterLink>
        <div class="opt" @click="iidx.settings">Game Settings</div>
        <div class="opt" @click="iidx.openCustomize">Customize Options</div>
        <div class="opt small" @click="iidx.generateBat">Generate Quick Launch BAT</div>
      </div>
    </div>
    <div class="game gitadora" :class="{'disable': gitadora.GameMeta.value == undefined}">
      <div class="background">
        <video loop autoplay muted src="../assets/gitadora.webm"></video>
      </div>
      <div class="text">
        GITADORA <br><br>
        <div v-if="gitadora.GameMeta.value != undefined && !gitadora.installed()" class="tip">
          Not Installed
        </div>
        <div v-if="gitadora.GameMeta.value != undefined && gitadora.installed()" class="tip">
          <small>Note: Server not currently supported</small>
        </div>
      </div>
      <div v-if="gitadora.GameMeta.value != undefined && gitadora.installed()" class="options">
        <div v-if="gitadora.GameVersionState.value == VersionState.Normal" class="opt" @click="gitadora.start">Start</div>
        <div v-if="gitadora.GameVersionState.value == VersionState.Need2UpdateLauncher" class="tip opt">
          Please Update Launcher
        </div>
        <div v-if="gitadora.GameVersionState.value == VersionState.Need2UpdateGame" class="tip opt">
          Please Update Game
        </div>
        <div class="opt" @click="gitadora.settings">Game Settings</div>
        <div v-if="gitadora.GameVersionState.value != VersionState.Need2UpdateGame" class="opt"
          @click="gitadora.updater">Updater</div>
        <div v-else class="blink opt" @click="gitadora.updater">Updater</div>
      </div>
    </div>
    <div></div>
  </main>
</template>

<style scoped lang="scss">
main {
  height: 100vh;
  display: flex;
  align-items: stretch;
  background-color: #000;
  background-image: url(@/assets/moai-bg.jpg);
  background-size: cover;
}

main>div {
  height: 100%;
  width: 100%;
  margin-left: -100px;
}

.game {
  transition: 0.5s ease;
  will-change: width;
  position: relative;
  clip-path: polygon(100px 0%, 100% 0%, calc(100% - 100px) 100%, 0% 100%);
  top: 0%;
}

.game.disable {
  width: 100px;
  top: 100%;
}

.game>.background {
  position: absolute;
  top: 0;
  left: 0;
  width: 100%;
  height: 100%;
  background-color: rgba(0, 0, 0, 0.5);
  background-position: center;
  filter: brightness(0.5);
  transition: 0.1s ease;
  z-index: -1;
}

.game>.text {
  text-align: center;
  margin: auto;
  margin-top: 225px;
  font-size: 22px;
  font-weight: 100;
  transition: 0.1s ease;
  color: #ccc;
  mix-blend-mode: screen;
}

.game:not(.disable):hover {
  width: 250%;
}

.game>.text>.tip {
  opacity: 0%;
  transition: 0.1s ease;
  padding: 1em;
}

.game:hover>.text>.tip {
  opacity: 100%;
}

.iidx>.background {
  background-image: url(@/assets/iidx.jpg);
}

.game>.options {
  cursor: pointer;
  transition: 0.1s ease;
  margin-top: 2em;
  font-size: 16px;
  opacity: 0%;
}

.game:hover>.options {
  opacity: 100%;
}

.game>.options>.opt {
  padding: 1em;
  text-align: center;
  display: block;
  color: #fff;
}

.game>.options>.gap {
  height: 2em;
}

.game>.options>.opt.small {
  padding: 0.5em;
  font-size: 10px;
}

@for $i from 1 through 10 {
  .game>.options>.opt:nth-child(#{$i}) {
    margin-right: calc($i * 9px);
  }
}

.game>.options>.opt:first-child {
  font-size: 24px;
}

.game>.options>.opt:hover {
  background-color: #fff;
  color: #000;
}

.sdvx>.background {
  background-image: url(@/assets/sdvx.jpg);
}

.game:hover>.background {
  filter: brightness(0.65);
}

.gitadora>.background {
  opacity: 0.8;
}

.gitadora>.background>video {
  position: absolute;
  right: -25%;
}

@keyframes blinkAnimation {
  0% {
    background-color: #00000000;
    color: #fff;
  }

  10% {
    background-color: #fff;
    color: #000;
  }

  20% {
    background-color: #00000000;
    color: #fff;
  }

  30% {
    background-color: #fff;
    color: #000;
  }

  40% {
    background-color: #00000000;
    color: #fff;
  }
}

.game>.options>.blink {
  animation: blinkAnimation 2s infinite;
}
</style>
