import React from 'react';

interface MainMenuProps {
  onStartGame: () => void;
}

export default function MainMenu({ onStartGame }: MainMenuProps) {
  return (
    <div className="menu-container">
      <div>
        <h1 className="menu-title">DOOM</h1>
        <p className="menu-subtitle">Linux Edition</p>
      </div>

      <div className="menu-buttons">
        <button className="menu-button" onClick={onStartGame}>
          Play Game
        </button>
        <button className="menu-button" onClick={() => alert('Controls:\nW/A/S/D - Move\nMouse - Look Around\nLeft Click - Fire\nE - Open Door\nP - Pause')}>
          Controls
        </button>
        <button className="menu-button" onClick={() => alert('Doom Demo - Linux Port\nVersion 1.0.0\n\nA classic DOOM-style shooter ported to Linux.')}>
          About
        </button>
        <button className="menu-button" onClick={() => window.close()}>
          Exit
        </button>
      </div>

      <div style={{ marginTop: 'auto', textAlign: 'center', opacity: 0.7 }}>
        <p>Use keyboard and mouse to play</p>
      </div>
    </div>
  );
}
