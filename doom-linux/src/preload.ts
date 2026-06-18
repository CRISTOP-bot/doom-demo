import { contextBridge, ipcRenderer } from 'electron';

contextBridge.exposeInMainWorld('doom', {
  init: (width: number, height: number) =>
    ipcRenderer.invoke('doom:init', width, height),
  update: (input: any) =>
    ipcRenderer.invoke('doom:update', input),
  render: () =>
    ipcRenderer.invoke('doom:render'),
  cleanup: () =>
    ipcRenderer.invoke('doom:cleanup'),
});

declare global {
  interface Window {
    doom: {
      init: (width: number, height: number) => Promise<any>;
      update: (input: any) => Promise<any>;
      render: () => Promise<any>;
      cleanup: () => Promise<any>;
    };
  }
}
