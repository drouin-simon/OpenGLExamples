#ifndef __DrawableTexture_h_
#define __DrawableTexture_h_

class DrawableTexture
{

public:

    DrawableTexture();
    ~DrawableTexture();
	
    bool Init( int width, int height );
	void Resize( int width, int height );
	void Release();
	
	void DrawToTexture( bool drawTo );
    void PasteToScreen( int x, int y, int width, int height );
    void PasteToScreen();
    void Clear( int x, int y, int width, int height );

    unsigned GetTexId() { return m_texId; }

protected:
	
	unsigned m_texId;
	unsigned m_fbId;
	int m_width;
	int m_height;
};

#endif
