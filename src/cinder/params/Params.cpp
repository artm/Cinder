/*
 Copyright (c) 2010, The Barbarian Group
 All rights reserved.

 Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and
	the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
	the following disclaimer in the documentation and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
*/

#include "cinder/app/App.h"
#include "cinder/params/Params.h"
#include "cinder/Filesystem.h"

#include "AntTweakBar.h"

#include <boost/assign/list_of.hpp>
#include <boost/foreach.hpp>

#include <ctype.h>

using namespace std;

namespace cinder { namespace params {

namespace {

#undef DELETE

#define SYNONYM(ck,ak) ((int)cinder::app::KeyEvent::KEY_ ## ck, TW_KEY_ ## ak)
#define HOMONYM(k) SYNONYM(k,k)
    std::map<int,TwKeySpecial> specialKeys = boost::assign::map_list_of
        HOMONYM(RIGHT)
        HOMONYM(LEFT)
        HOMONYM(BACKSPACE)
        HOMONYM(DELETE)
        HOMONYM(TAB)
        HOMONYM(F1)
        HOMONYM(F2)
        HOMONYM(F3)
        HOMONYM(F4)
        HOMONYM(F5)
        HOMONYM(F6)
        HOMONYM(F7)
        HOMONYM(F8)
        HOMONYM(F9)
        HOMONYM(F10)
        HOMONYM(F11)
        HOMONYM(F12)
        HOMONYM(F13)
        HOMONYM(F14)
        HOMONYM(F15)
        HOMONYM(HOME)
        HOMONYM(END)
        SYNONYM(PAGEUP,PAGE_UP)
        SYNONYM(PAGEDOWN,PAGE_DOWN)
        ;
#undef SYNONYM
#undef HOMONYM

bool mouseDown( app::MouseEvent event )
{
	TwMouseButtonID button;
	if( event.isLeft() )
		button = TW_MOUSE_LEFT;
	else if( event.isRight() )
		button = TW_MOUSE_RIGHT;
	else
		button = TW_MOUSE_MIDDLE;
	return TwMouseButton( TW_MOUSE_PRESSED, button ) != 0;
}

bool mouseUp( app::MouseEvent event )
{
	TwMouseButtonID button;
	if( event.isLeft() )
		button = TW_MOUSE_LEFT;
	else if( event.isRight() )
		button = TW_MOUSE_RIGHT;
	else
		button = TW_MOUSE_MIDDLE;
	return TwMouseButton( TW_MOUSE_RELEASED, button ) != 0;
}

bool mouseWheel( app::MouseEvent event )
{
	static float sWheelPos = 0;
	sWheelPos += event.getWheelIncrement();
	return TwMouseWheel( (int)(sWheelPos) ) != 0;
}

bool mouseMove( app::MouseEvent event )
{
	return TwMouseMotion( event.getX(), event.getY() ) != 0;
}

bool keyDown( app::KeyEvent event )
{
	int kmod = 0;
	if( event.isShiftDown() )
		kmod |= TW_KMOD_SHIFT;
	if( event.isControlDown() )
		kmod |= TW_KMOD_CTRL;
	if( event.isAltDown() )
		kmod |= TW_KMOD_ALT;
	return TwKeyPressed(
            (specialKeys.count( event.getCode() ) > 0)
                ? specialKeys[event.getCode()]
                : event.getChar(),
            kmod ) != 0;
}

bool resize( app::ResizeEvent event )
{
	TwWindowSize( event.getWidth(), event.getHeight() );
	return false;
}

void TW_CALL implStdStringToClient( std::string& destinationClientString, const std::string& sourceLibraryString )
{
  // copy strings from the library to the client app
  destinationClientString = sourceLibraryString;
}

class AntMgr {
  public:
	AntMgr() {
		if( ! TwInit( TW_OPENGL, NULL ) ) {
			throw Exception();
		}
		
		app::App::get()->registerMouseDown( mouseDown );
		app::App::get()->registerMouseUp( mouseUp );
		app::App::get()->registerMouseWheel( mouseWheel );		
		app::App::get()->registerMouseMove( mouseMove );
		app::App::get()->registerMouseDrag( mouseMove );
		app::App::get()->registerKeyDown( keyDown );
		app::App::get()->registerResize( resize );
	}
	
	~AntMgr() {
		TwTerminate();
	}
};

} // anonymous namespace

void initAntGl()
{
	static std::shared_ptr<AntMgr> mgr;
	if( ! mgr )
		mgr = std::shared_ptr<AntMgr>( new AntMgr );
}


InterfaceGl::InterfaceGl( const std::string &title, const Vec2i &size, const ColorA color )
{
	initAntGl();
	mBar = std::shared_ptr<TwBar>( TwNewBar( title.c_str() ), TwDeleteBar );
	char optionsStr[1024];
	sprintf( optionsStr, "`%s` size='%d %d' color='%d %d %d' alpha=%d", title.c_str(), size.x, size.y, (int)(color.r * 255), (int)(color.g * 255), (int)(color.b * 255), (int)(color.a * 255) );
	TwDefine( optionsStr );
	
	TwCopyStdStringToClientFunc( implStdStringToClient );
	
	m_id = name2id(title);
}

void InterfaceGl::draw()
{
	TwDraw();
}

void InterfaceGl::show( bool visible )
{
	int32_t visibleInt = ( visible ) ? 1 : 0;
	TwSetParam( mBar.get(), NULL, "visible", TW_PARAM_INT32, 1, &visibleInt );
}

void InterfaceGl::hide()
{
	int32_t visibleInt = 0;
	TwSetParam( mBar.get(), NULL, "visible", TW_PARAM_INT32, 1, &visibleInt );
}

bool InterfaceGl::isVisible() const
{
	int32_t visibleInt;
	TwGetParam( mBar.get(), NULL, "visible", TW_PARAM_INT32, 1, &visibleInt );
	return visibleInt != 0;
}

void InterfaceGl::implAddParam( const std::string &name, void *param, int type, const std::string &optionsStr, bool readOnly )
{
	if( readOnly )
		TwAddVarRO( mBar.get(), name.c_str(), (TwType)type, param, optionsStr.c_str() );
	else
		TwAddVarRW( mBar.get(), name.c_str(), (TwType)type, param, optionsStr.c_str() );
}

void InterfaceGl::addParam( const std::string &name, bool *param, const std::string &optionsStr, bool readOnly )
{
	implAddParam( name, param, TW_TYPE_BOOLCPP, optionsStr, readOnly );
} 

void InterfaceGl::addParam( const std::string &name, float *param, const std::string &optionsStr, bool readOnly )
{
	implAddParam( name, param, TW_TYPE_FLOAT, optionsStr, readOnly );
} 

void InterfaceGl::addParam( const std::string &name, int32_t *param, const std::string &optionsStr, bool readOnly )
{
	implAddParam( name, param, TW_TYPE_INT32, optionsStr, readOnly );
} 

void InterfaceGl::addParam( const std::string &name, Vec3f *param, const std::string &optionsStr, bool readOnly )
{
	implAddParam( name, param, TW_TYPE_DIR3F, optionsStr, readOnly );
} 

void InterfaceGl::addParam( const std::string &name, Quatf *param, const std::string &optionsStr, bool readOnly )
{
	implAddParam( name, param, TW_TYPE_QUAT4F, optionsStr, readOnly );
} 

void InterfaceGl::addParam( const std::string &name, Color *param, const std::string &optionsStr, bool readOnly )
{
	implAddParam( name, param, TW_TYPE_COLOR3F, optionsStr, readOnly );
} 

void InterfaceGl::addParam( const std::string &name, ColorA *param, const std::string &optionsStr, bool readOnly )
{
	implAddParam( name, param, TW_TYPE_COLOR4F, optionsStr, readOnly );
} 

void InterfaceGl::addParam( const std::string &name, std::string *param, const std::string &optionsStr, bool readOnly )
{
	implAddParam( name, param, TW_TYPE_STDSTRING, optionsStr, readOnly );
}

void InterfaceGl::addParam( const std::string &name, const std::vector<std::string> &enumNames, int *param, const std::string &optionsStr, bool readOnly )
{
	TwEnumVal *ev = new TwEnumVal[enumNames.size()];
	for( size_t v = 0; v < enumNames.size(); ++v ) {
		ev[v].Value = v;
		ev[v].Label = const_cast<char*>( enumNames[v].c_str() );
	}

	TwType evType = TwDefineEnum( (name + "EnumType").c_str(), ev, enumNames.size() );

	if( readOnly )
		TwAddVarRO( mBar.get(), name.c_str(), evType, param, optionsStr.c_str() );
	else
		TwAddVarRW( mBar.get(), name.c_str(), evType, param, optionsStr.c_str() );
		
	delete [] ev;
}

void InterfaceGl::addSeparator( const std::string &name, const std::string &optionsStr )
{
	TwAddSeparator( mBar.get(), name.c_str(), optionsStr.c_str() );
}

void InterfaceGl::addText( const std::string &name, const std::string &optionsStr )
{
	TwAddButton( mBar.get(), name.c_str(), NULL, NULL, optionsStr.c_str() );
}

namespace { // anonymous namespace
void TW_CALL implButtonCallback( void *clientData )
{
	std::function<void ()> *fn = reinterpret_cast<std::function<void ()>*>( clientData );
	(*fn)(); 
} 
} // anonymous namespace

void InterfaceGl::addButton( const std::string &name, const std::function<void ()> &callback, const std::string &optionsStr )
{
	std::shared_ptr<std::function<void ()> > callbackPtr( new std::function<void ()>( callback ) );
	mButtonCallbacks.push_back( callbackPtr );
	TwAddButton( mBar.get(), name.c_str(), implButtonCallback, (void*)callbackPtr.get(), optionsStr.c_str() );
}

void InterfaceGl::setOptions( const std::string &name, const std::string &optionsStr )
{
	std::string target = "`" + (std::string)TwGetBarName( mBar.get() ) + "`";
	if( !( name.empty() ) )
		target += "/`" + name + "`";

	TwDefine( ( target + " " + optionsStr ).c_str() );
}

std::string InterfaceGl::name2id( const std::string& name ) {
	std::string id = "";
	enum State { START, APPEND, UPCASE };
	State state(START);

	BOOST_FOREACH(char c, name) {
		switch(state) {
			case START:
				if (isalpha(c)) {
					id += c;
					state = APPEND;
				} else if (isdigit(c)) {
					id = "_" + c;
					state = APPEND;
				}
				break;
			case APPEND:
				if (isalnum(c)) {
					id += c;
				} else {
					state = UPCASE;
				}
				break;
			case UPCASE:
				if (islower(c)) {
					id += toupper(c);
					state = APPEND;
				} else if (isalnum(c)) {
					id += c;
					state = APPEND;
				}
				break;
		}
	}
	return id;
}

void InterfaceGl::load(const std::string& fname)
{
	filename() = fname;
	if (fs::exists( fname )) {
		root() = XmlTree( loadFile(fname) );
	}
}

void InterfaceGl::save() {
	BOOST_FOREACH(boost::function<void()> f, persistCallbacks())
		f();
	root().write( writeFile(filename()) );
}

void InterfaceGl::addPersistentSizeAndPosition()
{
	int size[2];
	TwGetParam( mBar.get(), NULL, "size", TW_PARAM_INT32, 2, size );

	std::string idW = name2id("width");
	size[0] = getXml().hasChild(idW)
		? getXml().getChild(idW).getValue((float)size[0])
		: size[0];

	std::string idH = name2id("height");
	size[1] = getXml().hasChild(idH)
		? getXml().getChild(idH).getValue((float)size[1])
		: size[1];

	TwSetParam( mBar.get(), NULL, "size", TW_PARAM_INT32, 2, size );

	int pos[2];
	TwGetParam( mBar.get(), NULL, "position", TW_PARAM_INT32, 2, pos );

	std::string idX = name2id("posx");
	pos[0] = getXml().hasChild(idX)
		? getXml().getChild(idX).getValue((float)pos[0])
		: pos[0];

	std::string idY = name2id("posy");
	pos[1] = getXml().hasChild(idY)
		? getXml().getChild(idY).getValue((float)pos[1])
		: pos[1];

	TwSetParam( mBar.get(), NULL, "position", TW_PARAM_INT32, 2, pos );

	int icon;
	TwGetParam( mBar.get(), NULL, "iconified", TW_PARAM_INT32, 1, &icon );

	std::string idIcon = name2id("icon");
	icon = getXml().hasChild(idIcon)
		? getXml().getChild(idIcon).getValue((int)icon)
		: icon;		

	TwSetParam( mBar.get(), NULL, "iconified", TW_PARAM_INT32, 1, &icon );		

	persistCallbacks().push_back(
			boost::bind( &InterfaceGl::persistSizeAndPosition, this) );
}

void InterfaceGl::persistSizeAndPosition()
{
	int size[2];
	TwGetParam( mBar.get(), NULL, "size", TW_PARAM_INT32, 2, size );

	std::string idW = name2id("width");
	if (!getXml().hasChild(idW))
		getXml().push_back(XmlTree(idW,""));
	getXml().getChild(idW).setValue(size[0]);

	std::string idH = name2id("height");
	if (!getXml().hasChild(idH))
		getXml().push_back(XmlTree(idH,""));
	getXml().getChild(idH).setValue(size[1]);

	int pos[2];
	TwGetParam( mBar.get(), NULL, "position", TW_PARAM_INT32, 2, pos );

	std::string idX = name2id("posx");
	if (!getXml().hasChild(idX))
		getXml().push_back(XmlTree(idX,""));
	getXml().getChild(idX).setValue(pos[0]);

	std::string idY = name2id("posy");
	if (!getXml().hasChild(idY))
		getXml().push_back(XmlTree(idY,""));
	getXml().getChild(idY).setValue(pos[1]);

	int icon;
	TwGetParam( mBar.get(), NULL, "iconified", TW_PARAM_INT32, 1, &icon );

	std::string idIcon = name2id("icon");
	if (!getXml().hasChild(idIcon))
		getXml().push_back(XmlTree(idIcon,""));
	getXml().getChild(idIcon).setValue(icon);
}

} } // namespace cinder::params
