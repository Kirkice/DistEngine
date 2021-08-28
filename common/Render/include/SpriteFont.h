/* $Header: /common/Render/include/SpriteFont.h			8/28/21 15:23pm Kirk 			    $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*						Project Name : DistEngine                                             *
*                                                                                             *
*						File Name : SpriteFont.h											  *
*                                                                                             *
*                       Programmer : Kirk                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#pragma once

#include "../../Core/include/PCH.h"
#include "../../Core/include/InterfacePointers.h"
#include "../../DistMath/include/DistMath.h"
#include "../../render/include/GraphicsTypes.h"

using namespace math;

namespace render
{
	class SpriteFont
	{

	public:

		enum FontStyle
		{
			Regular = 0,
			Bold = 1 << 0,
			Italic = 1 << 1,
			BoldItalic = Bold | Italic,
			Underline = 1 << 2,
			Strikeout = 1 << 3
		};

		struct CharDesc
		{
			float X;
			float Y;
			float Width;
			float Height;
		};

		static const wchar StartChar = '!';
		static const wchar EndChar = 127;
		static const uint64 NumChars = EndChar - StartChar;
		static const uint32 TexWidth = 1024;

		SpriteFont();
		~SpriteFont();

		// Lifetime
		void Initialize(const wchar* fontName, float fontSize, uint32 fontStyle, bool antiAliased);
		void Shutdown();

		Vector2 MeasureText(const wchar* text) const;

		// Accessors
		const CharDesc* CharDescriptors() const;
		const CharDesc& GetCharDescriptor(wchar character) const;
		float Size() const;
		const Texture* FontTexture() const;
		uint32 TextureWidth() const;
		uint32 TextureHeight() const;
		float SpaceWidth() const;
		float CharHeight() const;

	protected:

		Texture texture;
		CharDesc charDescs[NumChars];
		float size = 0.0f;
		uint32 texHeight = 0;
		float spaceWidth = 0.0f;
		float charHeight = 0.0f;
	};
}