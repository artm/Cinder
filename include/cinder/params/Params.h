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

#pragma once

#include "cinder/Color.h"
#include "cinder/Quaternion.h"
#include "cinder/Function.h"

#include <string>

typedef struct CTwBar TwBar;

namespace cinder {
    /** Lightweight GUI based on AntTweakBar.
     *
     * The class InterfaceGl wraps the excellend <a
     * href="http://www.antisphere.com/Wiki/tools:anttweakbar">AntTweakBar</a>
     * API that implements an overlay-style user interface. Multiple windows
     * can be created, semantically grouping different parameters.
     */
    namespace params {

/**
 * Single window, which may contain multiple parameter widgets. Multiple
 * InterfaceGl windows may be created, semantically grouping parameters. The
 * interface is mostly declarative: after the window is constructed multiple
 * parameters can be added to it and for each an appropriate widget will be
 * created. Keyboard and mouse events will be passed by cinder to the GUI, but
 * drawing has to be integrated into your application's draw() method: just call
 * static method InterfaceGL::draw() which will draw all defined windows.
 *
 * \section optionsStr Widget options
 *
 * addParam() methods accept a human readable parameter label, parameter address
 * (typed pointer), an option string further describing the parameter and a
 * boolean readonly flag. Type of the parameter address determines which widget
 * will be added (e.g. version with Color& argument creates a color picker,
 * version with Quatf& argument - a rotation widget etc).
 *
 * The format of the option string is described in <a
 * href="http://www.antisphere.com/Wiki/tools:anttweakbar:varparamsyntax">Variable
 * parameters syntax</a> section of AntTweakBar reference, but a short list of
 * options relevant when using with cinder follows:
 *
 * - \c label - param label (FIXME what's the difference with name argument to
 *   addParam()?)
 * - \c help - param help message, appears in a Help window
 * - \c group - move param to a named group
 * - \c min / \c max - minimum and maximum values of a numeric param
 * - \c step - numeric param's smallest change step
 * - \c precision - number of digits after decimal point displayed for a float
 *   or double type param
 * - \c hexa - display integer param in base 16 (hexadecimal) if true
 * - \c key - assign toggle/activate key to a boolean parameter/button
 *   respectively
 * - \c keyincr / \c keydecr - assign increase/decrease keys to a numeric
 *   parameter
 * - \c true / \c false - display true/false as provided strings (instead of
 *   default 'ON'/'OFF')
 * - \c opened - open or close group by default
 * - \c colormode - color widget mode - rgb or hls
 * - \c arrow - display an arrow instead of a sphere for a quaternion widget
 *   (the format is 'x y z' representing a direction vector)
 * - \c arrowcolor - color for the quaternion widget's arrow
 * - \c axisx / \c axisy / \c axisz - modify coordinate system representation
 *   for a quaternion widget
 * - \c showval - turn display of a value on or off
 */
class InterfaceGl {
 public:
    //! Default constructor, does not initialize the wrapper!
	InterfaceGl() {}
	InterfaceGl( const std::string &title, const Vec2i &size, const ColorA = ColorA( 0.3f, 0.3f, 0.3f, 0.4f ) );

	static void		draw();

	void	show( bool visible = true );
	void	hide();
	bool	isVisible() const;

	void	addParam( const std::string &name, bool *boolParam, const std::string &optionsStr = "", bool readOnly = false );
	void	addParam( const std::string &name, float *floatParam, const std::string &optionsStr = "", bool readOnly = false );
	void	addParam( const std::string &name, int32_t *intParam, const std::string &optionsStr = "", bool readOnly = false );
	void	addParam( const std::string &name, Vec3f *vectorParam, const std::string &optionsStr = "", bool readOnly = false );
	void	addParam( const std::string &name, Quatf *quatParam, const std::string &optionsStr = "", bool readOnly = false );
	void	addParam( const std::string &name, Color *quatParam, const std::string &optionsStr = "", bool readOnly = false );
	void	addParam( const std::string &name, ColorA *quatParam, const std::string &optionsStr = "", bool readOnly = false );
	void	addParam( const std::string &name, std::string *strParam, const std::string &optionsStr = "", bool readOnly = false );
	//! Adds enumerated parameter. The value corresponds to the indices of \a enumNames.
	void	addParam( const std::string &name, const std::vector<std::string> &enumNames, int *param, const std::string &optionsStr = "", bool readOnly = false );
	void	addSeparator( const std::string &name = "", const std::string &optionsStr = "" );
	void	addText( const std::string &name = "", const std::string &optionsStr = "" );
	void	addButton( const std::string &name, const std::function<void()> &callback, const std::string &optionsStr = "" );
	void	setOptions( const std::string &name = "", const std::string &optionsStr = "" );

 protected:
	void	implAddParam( const std::string &name, void *param, int type, const std::string &optionsStr, bool readOnly );

	std::shared_ptr<TwBar>							mBar;
	std::vector<std::shared_ptr<std::function<void()> > >	mButtonCallbacks;
};

} } // namespace cinder::params
