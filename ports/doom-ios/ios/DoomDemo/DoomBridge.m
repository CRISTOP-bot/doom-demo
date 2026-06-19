#import "DoomBridge.h"
#import <React/RCTLog.h>

@implementation DoomBridge

RCT_EXPORT_MODULE();

+ (BOOL)requiresMainQueueSetup {
  return YES;
}

RCT_EXPORT_METHOD(initializeGame:(RCTPromiseResolveBlock)resolve
                  rejecter:(RCTPromiseRejectBlock)reject) {
  @try {
    RCTLogInfo(@"[DoomBridge] Initializing game engine");
    resolve(@{@"status": @"initialized"});
  } @catch (NSException *exception) {
    reject(@"INIT_ERROR", exception.reason, nil);
  }
}

RCT_EXPORT_METHOD(updateInput:(NSDictionary *)input
                  resolver:(RCTPromiseResolveBlock)resolve
                  rejecter:(RCTPromiseRejectBlock)reject) {
  @try {
    RCTLogInfo(@"[DoomBridge] Input updated: %@", input);
    resolve(@{@"status": @"updated"});
  } @catch (NSException *exception) {
    reject(@"INPUT_ERROR", exception.reason, nil);
  }
}

RCT_EXPORT_METHOD(renderFrame:(RCTPromiseResolveBlock)resolve
                  rejecter:(RCTPromiseRejectBlock)reject) {
  @try {
    resolve(@{@"frame": @1});
  } @catch (NSException *exception) {
    reject(@"RENDER_ERROR", exception.reason, nil);
  }
}

RCT_EXPORT_METHOD(pauseGame:(RCTPromiseResolveBlock)resolve
                  rejecter:(RCTPromiseRejectBlock)reject) {
  @try {
    RCTLogInfo(@"[DoomBridge] Game paused");
    resolve(@{@"status": @"paused"});
  } @catch (NSException *exception) {
    reject(@"PAUSE_ERROR", exception.reason, nil);
  }
}

RCT_EXPORT_METHOD(resumeGame:(RCTPromiseResolveBlock)resolve
                  rejecter:(RCTPromiseRejectBlock)reject) {
  @try {
    RCTLogInfo(@"[DoomBridge] Game resumed");
    resolve(@{@"status": @"resumed"});
  } @catch (NSException *exception) {
    reject(@"RESUME_ERROR", exception.reason, nil);
  }
}

@end
