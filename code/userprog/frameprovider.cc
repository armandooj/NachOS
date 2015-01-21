#ifdef CHANGED
#include "frameprovider.h"

FrameProvider::FrameProvider(int numFrames) {
  framesBitMap = new BitMap(numFrames);
  framesBitMap->Mark(0);
  lock = new Lock("FrameProvider lock");
}

FrameProvider::~FrameProvider() {
  delete(framesBitMap);
}

int FrameProvider::GetEmptyFrame() {
  // TODO Random()
  lock->Acquire();
  int frame = framesBitMap->Find();
  lock->Release();
  return frame;
}

void FrameProvider::ReleaseFrame(int frame) {
  framesBitMap->Clear(frame);
}

int FrameProvider::NumAvailFrame() {
  lock->Acquire();
  int number = framesBitMap->NumClear();
  lock->Release();
  return number;
}


#endif