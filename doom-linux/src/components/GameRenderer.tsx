import React, { useEffect, useRef, forwardRef, useImperativeHandle } from 'react';

interface GameRendererProps {
  isPaused: boolean;
  onGameOver: () => void;
  onScoreChange: (score: number) => void;
  onHealthChange: (health: number) => void;
}

const GameRenderer = forwardRef<any, GameRendererProps>(
  ({ isPaused, onGameOver, onScoreChange, onHealthChange }, ref) => {
    const canvasRef = useRef<HTMLCanvasElement>(null);
    const gameStateRef = useRef({
      score: 0,
      health: 100,
      isRunning: false,
      keys: {} as Record<string, boolean>,
      mouseX: 0,
      mouseY: 0,
    });

    useImperativeHandle(ref, () => ({
      init: async () => {
        gameStateRef.current.isRunning = true;
        gameStateRef.current.health = 100;
        gameStateRef.current.score = 0;
        
        if (window.doom) {
          try {
            await window.doom.init(1280, 960);
            startGameLoop();
          } catch (error) {
            console.error('Failed to initialize game:', error);
          }
        }
      },
      cleanup: async () => {
        gameStateRef.current.isRunning = false;
        if (window.doom) {
          try {
            await window.doom.cleanup();
          } catch (error) {
            console.error('Failed to cleanup game:', error);
          }
        }
      },
    }));

    const startGameLoop = () => {
      const gameLoop = async () => {
        if (!gameStateRef.current.isRunning) return;

        if (!isPaused && window.doom) {
          try {
            // Actualizar entrada
            const input = {
              w: gameStateRef.current.keys['w'] || gameStateRef.current.keys['W'] ? 1 : 0,
              a: gameStateRef.current.keys['a'] || gameStateRef.current.keys['A'] ? 1 : 0,
              s: gameStateRef.current.keys['s'] || gameStateRef.current.keys['S'] ? 1 : 0,
              d: gameStateRef.current.keys['d'] || gameStateRef.current.keys['D'] ? 1 : 0,
              mouseX: gameStateRef.current.mouseX,
              mouseY: gameStateRef.current.mouseY,
            };

            await window.doom.update(input);
            const frameData = await window.doom.render();

            if (frameData) {
              gameStateRef.current.score = frameData.score || gameStateRef.current.score;
              gameStateRef.current.health = frameData.health || gameStateRef.current.health;

              onScoreChange(gameStateRef.current.score);
              onHealthChange(gameStateRef.current.health);

              if (gameStateRef.current.health <= 0) {
                gameStateRef.current.isRunning = false;
                onGameOver();
              }
            }
          } catch (error) {
            console.error('Game loop error:', error);
          }
        }

        requestAnimationFrame(gameLoop);
      };

      gameLoop();
    };

    useEffect(() => {
      const handleKeyDown = (e: KeyboardEvent) => {
        gameStateRef.current.keys[e.key] = true;
      };

      const handleKeyUp = (e: KeyboardEvent) => {
        gameStateRef.current.keys[e.key] = false;
      };

      const handleMouseMove = (e: MouseEvent) => {
        if (canvasRef.current) {
          const rect = canvasRef.current.getBoundingClientRect();
          gameStateRef.current.mouseX = e.clientX - rect.left;
          gameStateRef.current.mouseY = e.clientY - rect.top;
        }
      };

      const handleMouseClick = (e: MouseEvent) => {
        gameStateRef.current.keys['click'] = true;
        setTimeout(() => {
          gameStateRef.current.keys['click'] = false;
        }, 100);
      };

      window.addEventListener('keydown', handleKeyDown);
      window.addEventListener('keyup', handleKeyUp);
      window.addEventListener('mousemove', handleMouseMove);
      window.addEventListener('click', handleMouseClick);

      return () => {
        window.removeEventListener('keydown', handleKeyDown);
        window.removeEventListener('keyup', handleKeyUp);
        window.removeEventListener('mousemove', handleMouseMove);
        window.removeEventListener('click', handleMouseClick);
      };
    }, []);

    return (
      <div className="game-renderer">
        <canvas
          ref={canvasRef}
          className="game-canvas"
          width={1280}
          height={960}
        />
      </div>
    );
  }
);

GameRenderer.displayName = 'GameRenderer';

export default GameRenderer;
