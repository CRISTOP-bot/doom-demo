import { useState, useEffect, useRef } from 'react';
import './App.css';
import GameRenderer from './components/GameRenderer';
import MainMenu from './components/MainMenu';
import GameControls from './components/GameControls';

type GameState = 'menu' | 'playing' | 'paused' | 'game-over';

export default function App() {
  const [gameState, setGameState] = useState<GameState>('menu');
  const [score, setScore] = useState(0);
  const [health, setHealth] = useState(100);
  const gameRef = useRef<any>(null);

  const handleStartGame = async () => {
    setGameState('playing');
    setScore(0);
    setHealth(100);
    
    if (gameRef.current) {
      await gameRef.current.init();
    }
  };

  const handlePauseGame = () => {
    setGameState(gameState === 'paused' ? 'playing' : 'paused');
  };

  const handleResumeGame = () => {
    setGameState('playing');
  };

  const handleGameOver = () => {
    setGameState('game-over');
  };

  const handleReturnToMenu = () => {
    setGameState('menu');
    if (gameRef.current) {
      gameRef.current.cleanup();
    }
  };

  return (
    <div className="app">
      {gameState === 'menu' && (
        <MainMenu onStartGame={handleStartGame} />
      )}

      {(gameState === 'playing' || gameState === 'paused') && (
        <div className="game-container">
          <GameRenderer
            ref={gameRef}
            isPaused={gameState === 'paused'}
            onGameOver={handleGameOver}
            onScoreChange={setScore}
            onHealthChange={setHealth}
          />

          <div className="game-hud">
            <div className="hud-stat">
              <span className="hud-label">Health:</span>
              <div className="health-bar">
                <div
                  className="health-fill"
                  style={{ width: `${Math.max(0, health)}%` }}
                />
              </div>
              <span className="hud-value">{Math.max(0, health)}</span>
            </div>

            <div className="hud-stat">
              <span className="hud-label">Score:</span>
              <span className="hud-value">{score}</span>
            </div>

            <div className="hud-controls">
              <button
                className="hud-button"
                onClick={handlePauseGame}
              >
                {gameState === 'paused' ? 'RESUME' : 'PAUSE'}
              </button>
              <button
                className="hud-button"
                onClick={handleReturnToMenu}
              >
                MENU
              </button>
            </div>
          </div>

          {gameState === 'paused' && (
            <div className="pause-overlay">
              <div className="pause-menu">
                <h1>GAME PAUSED</h1>
                <button onClick={handleResumeGame}>Resume</button>
                <button onClick={handleReturnToMenu}>Return to Menu</button>
              </div>
            </div>
          )}
        </div>
      )}

      {gameState === 'game-over' && (
        <div className="game-over-screen">
          <h1>GAME OVER</h1>
          <p>Final Score: {score}</p>
          <button onClick={handleReturnToMenu}>Return to Menu</button>
        </div>
      )}

      <GameControls />
    </div>
  );
}
