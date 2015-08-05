#ifndef __drwDrawableTexture_h_
#define __drwDrawableTexture_h_

class drwDrawableTexture
{

public:

	drwDrawableTexture();
	~drwDrawableTexture();
	
	bool Init( int width, int height );
	void Resize( int width, int height );
    void Upload( unsigned char * buffer );
    void Upload( unsigned pboId );
    void Download( unsigned char * buffer );
    void Download( unsigned pboId );
	void Release();
	
	void DrawToTexture( bool drawTo );
    void PasteToScreen( int x, int y, int width, int height );
    void PasteToScreen();
    void Clear( int x, int y, int width, int height );
    unsigned GetTextureId() { return m_texId; }
    unsigned GetFramebufferId() { return m_fbId; }

protected:
	
	unsigned m_texId;
	unsigned m_fbId;
	int m_width;
	int m_height;
};

#endif
