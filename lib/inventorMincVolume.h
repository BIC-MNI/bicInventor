#ifndef __INVENTORMINCVOLUME__
#define __INVENTORMINCVOLUME__

#include <Inventor/nodes/SoTexture2.h>

#include <mniVolume.h>

class inventorMincVolume : public mniVolume {
protected:
  int currentX;
  int currentY;
  int currentZ;
  SoTexture2 *textureNode;
  unsigned char *textureArray;

public:
  // constructor
  inventorMincVolume(char *filename);
  void setTransverseSliceTexture( int zslice );
  void setSagittalSliceTexture( int xslice );
  void setCoronalSliceTexture( int yslice );

  // convenience functions
  void setNextTransverseSliceTexture();
  void setNextSagittalSliceTexture();
  void setNextCoronalSliceTexture();

  void setPreviousTransverseSliceTexture();
  void setPreviousSagittalSliceTexture();
  void setPreviousCoronalSliceTexture();

  int getCurrentX() { return this->currentX;};
  int getCurrentY() {return this->currentY;};
  int getCurrentZ() {return this->currentZ;};
  SoTexture2* getTexture() { return this->textureNode; };

};

#endif // __INVENTORMINCVOLUME__

