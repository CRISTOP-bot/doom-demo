import { app, BrowserWindow, Menu, ipcMain } from 'electron';
import path from 'path';
import isDev from 'electron-is-dev';

let mainWindow: BrowserWindow | null = null;

const createWindow = () => {
  mainWindow = new BrowserWindow({
    width: 1280,
    height: 960,
    webPreferences: {
      preload: path.join(__dirname, 'preload.js'),
      nodeIntegration: false,
      contextIsolation: true,
      enableRemoteModule: false,
    },
    icon: path.join(__dirname, '../assets/icon.png'),
  });

  const startUrl = isDev
    ? 'http://localhost:3000'
    : `file://${path.join(__dirname, '../renderer/index.html')}`;

  mainWindow.loadURL(startUrl);

  if (isDev) {
    mainWindow.webContents.openDevTools();
  }

  mainWindow.on('closed', () => {
    mainWindow = null;
  });
};

app.on('ready', createWindow);

app.on('window-all-closed', () => {
  if (process.platform !== 'darwin') {
    app.quit();
  }
});

app.on('activate', () => {
  if (mainWindow === null) {
    createWindow();
  }
});

// IPC Handlers para comunicación con el motor C
ipcMain.handle('doom:init', async (event, width: number, height: number) => {
  try {
    const doomModule = require('../build/Release/doom_native.node');
    return doomModule.init(width, height);
  } catch (error) {
    console.error('Error initializing Doom:', error);
    return { error: (error as Error).message };
  }
});

ipcMain.handle('doom:update', async (event, input: any) => {
  try {
    const doomModule = require('../build/Release/doom_native.node');
    return doomModule.update(input);
  } catch (error) {
    console.error('Error updating Doom:', error);
    return { error: (error as Error).message };
  }
});

ipcMain.handle('doom:render', async () => {
  try {
    const doomModule = require('../build/Release/doom_native.node');
    return doomModule.render();
  } catch (error) {
    console.error('Error rendering Doom:', error);
    return { error: (error as Error).message };
  }
});

ipcMain.handle('doom:cleanup', async () => {
  try {
    const doomModule = require('../build/Release/doom_native.node');
    return doomModule.cleanup();
  } catch (error) {
    console.error('Error cleaning up Doom:', error);
    return { error: (error as Error).message };
  }
});

// Menu
const template: any[] = [
  {
    label: 'File',
    submenu: [
      {
        label: 'Exit',
        accelerator: 'CmdOrCtrl+Q',
        click: () => {
          app.quit();
        },
      },
    ],
  },
  {
    label: 'View',
    submenu: [
      {
        label: 'Reload',
        accelerator: 'CmdOrCtrl+R',
        click: () => {
          mainWindow?.reload();
        },
      },
      {
        label: 'Toggle Developer Tools',
        accelerator: 'CmdOrCtrl+Shift+I',
        click: () => {
          mainWindow?.webContents.toggleDevTools();
        },
      },
    ],
  },
];

const menu = Menu.buildFromTemplate(template);
Menu.setApplicationMenu(menu);
