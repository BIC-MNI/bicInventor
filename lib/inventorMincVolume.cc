#include "inventorMincVolume.h"

inventorMincVolume::inventorMincVolume(char *filename)
  : mniVolume(filename, 0, 255, 3, XYZdimOrder, NC_BYTE, FALSE, TRUE, NULL) {
  
  //  this->textureNode = (SoTexture2 *)SoNode::getByName("TransverseTexture");
  this->textureNode = new SoTexture2;
  this->textureArray = new unsigned char[this->sizes[0] * this->sizes[1]];

  // set the current{XYZ} variables
  this->currentX = (int) this->sizes[0] / 2;
  this->currentY = (int) this->sizes[1] / 2;
  this->currentZ = (int) this->sizes[2] / 2;

}
void inventorMincVolume::setTransverseSliceTexture( int zslice ) {
  this->currentZ = zslice;

  for (int x=0; x < this->sizes[0]; x++) {
    for (int y=0; y < this->sizes[1]; y++) {
      // get voxel rather than real value - uses minc's scaling this way
      this->textureArray[(x*this->sizes[0]) + y] = 
        (unsigned char) get_volume_voxel_value(this->volume,x,y,this->currentZ,
                                               0,0);
    }
  }
  this->textureNode->image.setValue(SbVec2s(this->sizes[0], this->sizes[1]),
                                    1, this->textureArray);
}

  
void inventorMincVolume::setSagittalSliceTexture( int xslice ) { }
void inventorMincVolume::setCoronalSliceTexture( int yslice ) { }

void inventorMincVolume::setNextTransverseSliceTexture() {
  this->currentZ++;
  this->setTransverseSliceTexture( this->currentZ );
}

void inventorMincVolume::setPreviousTransverseSliceTexture() {
  this->currentZ--;
  this->setTransverseSliceTexture( this->currentZ );
}

inline void inventorMincVolume::setNextSagittalSliceTexture() { }
inline void inventorMincVolume::setPreviousSagittalSliceTexture() { }
inline void inventorMincVolume::setNextCoronalSliceTexture() { }
inline void inventorMincVolume::setPreviousCoronalSliceTexture() { }



