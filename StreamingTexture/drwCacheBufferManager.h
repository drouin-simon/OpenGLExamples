#ifndef __drwCacheBufferManager_h_
#define __drwCacheBufferManager_h_

#include <vector>
#include "drwDrawableTexture.h"

class PixelBuffer
{
public:
    PixelBuffer() : width(0), height(0), buffer(0) {}
    ~PixelBuffer() { delete [] buffer; }
    void AllocateAndClear( int width, int height );
    int Width() { return width; }
    int Height() { return height; }
    unsigned char * Buffer() { return buffer; }
protected:
    int width;
    int height;
    unsigned char * buffer;
};

class drwCacheBufferManager
{

public:

    drwCacheBufferManager();
    ~drwCacheBufferManager();
    void Init( int width, int height, int nbBuffers, bool usePbo );
    void Clear();
    void SetCurrentFrame( int index );
    unsigned GetCurrentTextureId();
    unsigned GetCurrentFramebufferId();
    void DrawCurrentTexture();

protected:

    bool m_usePbo;
    int m_numberOfBuffers;
    int m_width;
    int m_height;
    drwDrawableTexture * m_prevFrameTexture;
    drwDrawableTexture * m_currentFrameTexture;
    drwDrawableTexture * m_nextFrameTexture;

    int m_pendingDownloadFrame;
    unsigned m_downloadPixBuffer;
    unsigned m_uploadPixBuffer;
    
    int m_currentBuffer;
    std::vector<PixelBuffer> m_buffers;
    unsigned * m_pixelBufferObjects;

};

#endif
