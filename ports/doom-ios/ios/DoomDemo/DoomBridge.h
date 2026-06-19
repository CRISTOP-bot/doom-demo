#ifndef DoomBridge_h
#define DoomBridge_h

#import <Foundation/Foundation.h>
#import <React/RCTBridgeModule.h>

@interface DoomBridge : NSObject <RCTBridgeModule>

+ (BOOL)requiresMainQueueSetup;
- (void)initializeGame;
- (void)updateInput:(NSDictionary *)input;
- (void)renderFrame;
- (void)pauseGame;
- (void)resumeGame;

@end

#endif
