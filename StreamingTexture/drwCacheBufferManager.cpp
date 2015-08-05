#include "drwCacheBufferManager.h"
#include "drwDrawableTexture.h"
#include "GL/glew.h"
#include <string.h>

void PixelBuffer::AllocateAndClear( int width, int height )
{
    if( buffer )
        delete [] buffer;
    this->width = width;
    this->height = height;
    int bufferSize = width * height * 4;
    buffer = new unsigned char[ bufferSize ];
    memset( (void*)buffer, 0, bufferSize );
}

drwCacheBufferManager::drwCacheBufferManager()
{
    m_width = 0;
    m_height = 0;
    m_prevFrameTexture = 0;
    m_currentFrameTexture = 0;
    m_nextFrameTexture = 0;
    m_pendingDownloadFrame = -1;
    m_downloadPixBuffer = 0;
    m_uploadPixBuffer = 0;
}

drwCacheBufferManager::~drwCacheBufferManager()
{
    Clear();
}

void drwCacheBufferManager::Init( int width, int height, int nbBuffers, bool usePbo )
{
    Clear();
    
    m_width = width;
    m_height = height;
    
    m_prevFrameTexture = new drwDrawableTexture;
    m_currentFrameTexture = new drwDrawableTexture;
    m_nextFrameTexture = new drwDrawableTexture;
    
    m_usePbo = usePbo;
    m_numberOfBuffers = nbBuffers;
    
    // Create GL textures to hold frames
    m_prevFrameTexture->Resize( width, height );
    m_currentFrameTexture->Resize( width, height );
    m_nextFrameTexture->Resize( width, height );
    
    m_buffers.resize( nbBuffers );
    for( unsigned i = 0; i < m_buffers.size(); ++i )
    {
        m_buffers[i].AllocateAndClear( width, height );
    }
    
    if( m_usePbo )
    {
        unsigned bufferSize = width * height * 4;
        
        glGenBuffers( 1, &m_downloadPixBuffer );
        glBindBuffer( GL_PIXEL_PACK_BUFFER, m_downloadPixBuffer );
        glBufferData( GL_PIXEL_PACK_BUFFER, bufferSize, 0, GL_DYNAMIC_READ );
        glBindBuffer( GL_PIXEL_PACK_BUFFER, 0 );
        
        glGenBuffers( 1, &m_uploadPixBuffer );
        glBindBuffer( GL_PIXEL_UNPACK_BUFFER, m_uploadPixBuffer );
        glBufferData( GL_PIXEL_UNPACK_BUFFER, bufferSize, 0, GL_DYNAMIC_DRAW );
        glBindBuffer( GL_PIXEL_UNPACK_BUFFER, 0 );
    }
}

void drwCacheBufferManager::Clear()
{
    delete m_prevFrameTexture;
    delete m_currentFrameTexture;
    delete m_nextFrameTexture;
    
    m_buffers.clear();
    if( m_usePbo )
    {
        glDeleteBuffers( 1, &m_downloadPixBuffer );
        glDeleteBuffers( 1, &m_uploadPixBuffer );
    }
}

void drwCacheBufferManager::SetCurrentFrame( int index )
{
    m_currentBuffer = index;
    int prevBuffer = m_currentBuffer - 1;
    if( prevBuffer < 0 )
        prevBuffer = m_numberOfBuffers - 1;
    int nextBuffer = m_currentBuffer + 1;
    if( nextBuffer >= m_numberOfBuffers )
        nextBuffer = 0;
    
    drwDrawableTexture * tempTex = m_prevFrameTexture;
    m_prevFrameTexture = m_currentFrameTexture;
    m_currentFrameTexture = m_nextFrameTexture;
    m_nextFrameTexture = tempTex;
    
    int bufferSize = m_width * m_height * 4;
    
    // download prev frame from texture
    if( m_usePbo )
    {
        if( m_pendingDownloadFrame != -1 )
        {
            // copy PBO data to main buffer at index m_pendingDownloadFrame
            glBindBuffer( GL_PIXEL_PACK_BUFFER, m_downloadPixBuffer );
            unsigned char * buffer = (unsigned char*)glMapBuffer( GL_PIXEL_PACK_BUFFER, GL_READ_ONLY );
            memcpy( m_buffers[ m_pendingDownloadFrame ].Buffer(), buffer, bufferSize );
            glUnmapBuffer( GL_PIXEL_PACK_BUFFER );
            glBindBuffer( GL_PIXEL_PACK_BUFFER, 0 );
        }
        m_prevFrameTexture->Download( m_downloadPixBuffer );
        m_pendingDownloadFrame = prevBuffer;
    }
    else
        m_prevFrameTexture->Download( m_buffers[ prevBuffer ].Buffer() );    

    // upload new frame to texture
    if( m_usePbo )
    {
        // Copy frame data to PBO
        glBindBuffer( GL_PIXEL_UNPACK_BUFFER, m_uploadPixBuffer );
        //glBufferSubData( GL_PIXEL_UNPACK_BUFFER, 0, bufferSize, m_buffers[ nextBuffer ].Buffer() );
        unsigned char * buffer = (unsigned char*)glMapBuffer( GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY );
        memcpy( buffer, m_buffers[ m_pendingDownloadFrame ].Buffer(), bufferSize );
        glUnmapBuffer( GL_PIXEL_UNPACK_BUFFER );
        glBindBuffer( GL_PIXEL_UNPACK_BUFFER, 0 );
        
        // Start an upload of PBO to texture
        m_nextFrameTexture->Upload( m_uploadPixBuffer );
    }
    else
        m_nextFrameTexture->Upload( m_buffers[ nextBuffer ].Buffer() );
    
    
}

unsigned drwCacheBufferManager::GetCurrentTextureId()
{
    return m_currentFrameTexture->GetTextureId();
}

unsigned drwCacheBufferManager::GetCurrentFramebufferId()
{
    return m_currentFrameTexture->GetFramebufferId();
}

void drwCacheBufferManager::DrawCurrentTexture()
{
    m_currentFrameTexture->PasteToScreen();
}
