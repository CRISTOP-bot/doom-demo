import { NativeModules } from 'react-native';

export interface Spec {
  init(width: number, height: number): Promise<boolean>;
  update(w: number, a: number, s: number, d: number, left: number, right: number, space: number, e: number, tab: number, r: number, mouseDx: number): Promise<void>;
  getGameState(): Promise<{hp: number; ammo: number; score: number; gameOver: boolean; level: number}>;
  render(): Promise<void>;
  reset(): Promise<void>;
  cleanup(): Promise<void>;
  setViewport(w: number, h: number): Promise<void>;
}

const DoomModule = NativeModules.DoomModule as Spec;

console.log("Native:", DoomModule)

export default DoomModule;