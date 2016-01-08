
#include "cDirectX9LLShader.h"

namespace nextar
{

class cDirectX9ToonShader : public cDirectX9LLShader
{
public:

	cDirectX9ToonShader( iShader* pkBase = 0 );
	~cDirectX9ToonShader();

	virtual void set( cMaterial*, bool reset );
};

}