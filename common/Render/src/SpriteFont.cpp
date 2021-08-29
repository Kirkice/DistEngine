/* $Header: /common/Render/include/SpriteFont.h			8/28/21 15:23pm Kirk 			    $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*						Project Name : DistEngine                                             *
*                                                                                             *
*						File Name : SpriteFont.h									          *
*                                                                                             *
*                       Programmer : Kirk                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#include "../../Core/include/PCH.h"
#include "../../Core/include/Utility.h"

#include "../../Render/include/SpriteFont.h"
#include "../../Render/include/Textures.h"

using namespace Gdiplus;
using std::wstring;
using std::vector;

namespace render
{
	SpriteFont::SpriteFont()
	{
	}

	SpriteFont::~SpriteFont()
	{
	}

	void SpriteFont::Initialize(const wchar* fontName, float fontSize, uint32 fontStyle, bool antiAliased)
	{
		texture.Shutdown();

		size = fontSize;

		TextRenderingHint hint = antiAliased ? TextRenderingHintAntiAliasGridFit : TextRenderingHintSingleBitPerPixelGridFit;

		// Init GDI+
		ULONG_PTR token = 0;
		GdiplusStartupInput startupInput(nullptr, true, true);
		GdiplusStartupOutput startupOutput;
		GdiPlusCall(GdiplusStartup(&token, &startupInput, &startupOutput));

		try
		{
			// Create the font
			Gdiplus::Font font(fontName, fontSize, fontStyle, UnitPixel, nullptr);

			// Check for error during construction
			GdiPlusCall(font.GetLastStatus());

			// Create a temporary Bitmap and Graphics for figuring out the rough size required
			// for drawing all of the characters
			int32 bmSize = static_cast<int32>(fontSize * NumChars * 2) + 1;
			Bitmap sizeBitmap(bmSize, bmSize, PixelFormat32bppARGB);
			GdiPlusCall(sizeBitmap.GetLastStatus());

			Graphics sizeGraphics(&sizeBitmap);
			GdiPlusCall(sizeGraphics.GetLastStatus());
			GdiPlusCall(sizeGraphics.SetTextRenderingHint(hint));

			charHeight = font.GetHeight(&sizeGraphics) * 1.5f;

			wchar allChars[NumChars + 1];
			for (wchar i = 0; i < NumChars; ++i)
				allChars[i] = i + StartChar;
			allChars[NumChars] = 0;

			RectF sizeRect;
			GdiPlusCall(sizeGraphics.MeasureString(allChars, NumChars, &font, PointF(0, 0), &sizeRect));
			int32 numRows = static_cast<int32>(sizeRect.Width / TexWidth) + 1;
			texHeight = static_cast<int32>(numRows * charHeight) + 1;

			// Create a temporary Bitmap and Graphics for drawing the characters one by one
			int32 tempSize = static_cast<int32>(fontSize * 2);
			Bitmap drawBitmap(tempSize, tempSize, PixelFormat32bppARGB);
			GdiPlusCall(drawBitmap.GetLastStatus());

			Graphics drawGraphics(&drawBitmap);
			GdiPlusCall(drawGraphics.GetLastStatus());
			GdiPlusCall(drawGraphics.SetTextRenderingHint(hint));

			// Create a temporary Bitmap + Graphics for creating a full character set
			Bitmap textBitmap(TexWidth, texHeight, PixelFormat32bppARGB);
			GdiPlusCall(textBitmap.GetLastStatus());

			Graphics textGraphics(&textBitmap);
			GdiPlusCall(textGraphics.GetLastStatus());
			GdiPlusCall(textGraphics.Clear(Color(0, 255, 255, 255)));
			GdiPlusCall(textGraphics.SetCompositingMode(CompositingModeSourceCopy));

			// Solid brush for text rendering
			SolidBrush brush(Color(255, 255, 255, 255));
			GdiPlusCall(brush.GetLastStatus());

			// Draw all of the characters, and copy them to the full character set
			wchar charString[2];
			charString[1] = 0;
			int32 currentX = 0;
			int32 currentY = 0;
			for (uint64 i = 0; i < NumChars; ++i)
			{
				charString[0] = static_cast<wchar>(i + StartChar);

				// Draw the character
				GdiPlusCall(drawGraphics.Clear(Color(0, 255, 255, 255)));
				GdiPlusCall(drawGraphics.DrawString(charString, 1, &font, PointF(0, 0), &brush));

				// Figure out the amount of blank space before the character
				int32 minX = 0;
				for (int32 x = 0; x < tempSize; ++x)
				{
					for (int32 y = 0; y < tempSize; ++y)
					{
						Color color;
						GdiPlusCall(drawBitmap.GetPixel(x, y, &color));
						if (color.GetAlpha() > 0)
						{
							minX = x;
							x = tempSize;
							break;
						}
					}
				}

				// Figure out the amount of blank space after the character
				int32 maxX = tempSize - 1;
				for (int32 x = tempSize - 1; x >= 0; --x)
				{
					for (int32 y = 0; y < tempSize; ++y)
					{
						Color color;
						GdiPlusCall(drawBitmap.GetPixel(x, y, &color));
						if (color.GetAlpha() > 0)
						{
							maxX = x;
							x = -1;
							break;
						}
					}
				}

				int32 charWidth = maxX - minX + 1;

				// Figure out if we need to move to the next row
				if (currentX + charWidth >= TexWidth)
				{
					currentX = 0;
					currentY += static_cast<int32>(charHeight) + 1;
				}

				// Fill out the structure describing the character position
				charDescs[i].X = static_cast<float>(currentX);
				charDescs[i].Y = static_cast<float>(currentY);
				charDescs[i].Width = static_cast<float>(charWidth);
				charDescs[i].Height = static_cast<float>(charHeight);

				// Copy the character over
				int32 height = static_cast<int32>(charHeight + 1);
				GdiPlusCall(textGraphics.DrawImage(&drawBitmap, currentX, currentY, minX, 0, charWidth, height, UnitPixel));

				currentX += charWidth + 1;
			}

			// Figure out the width of a space character
			charString[0] = ' ';
			charString[1] = 0;
			GdiPlusCall(drawGraphics.MeasureString(charString, 1, &font, PointF(0, 0), &sizeRect));
			spaceWidth = sizeRect.Width;

			// Lock the bitmap for direct memory access
			BitmapData bmData;
			Gdiplus::Rect rect = Gdiplus::Rect(0, 0, TexWidth, texHeight);
			GdiPlusCall(textBitmap.LockBits(&rect, ImageLockModeRead, PixelFormat32bppARGB, &bmData));

			Create2DTexture(texture, TexWidth, texHeight, 1, 1, DXGI_FORMAT_B8G8R8A8_UNORM, false, bmData.Scan0);

			GdiPlusCall(textBitmap.UnlockBits(&bmData));
		}
		catch (GdiPlusException e)
		{
			// Shutdown GDI+
			if (token != NULL)
				GdiplusShutdown(token);
			throw e;
		}

		// Shutdown GDI+
		GdiplusShutdown(token);
	}

	void SpriteFont::Shutdown()
	{
		texture.Shutdown();
	}

	const SpriteFont::CharDesc* SpriteFont::CharDescriptors() const
	{
		return charDescs;
	}

	const SpriteFont::CharDesc& SpriteFont::GetCharDescriptor(wchar character) const
	{
		Assert_(character >= StartChar && character <= EndChar);
		return charDescs[character - StartChar];
	}

	float SpriteFont::Size() const
	{
		return size;
	}

	uint32 SpriteFont::TextureWidth() const
	{
		return TexWidth;
	}

	uint32 SpriteFont::TextureHeight() const
	{
		return texHeight;
	}

	float SpriteFont::SpaceWidth() const
	{
		return spaceWidth;
	}

	float SpriteFont::CharHeight() const
	{
		return charHeight;
	}

	const Texture* SpriteFont::FontTexture() const
	{
		return &texture;
	}

	Vector2 SpriteFont::MeasureText(const wchar* text) const
	{
		Vector2 textSize = Vector2(0.0f, 0.0f);
		Vector2 curPos = Vector2(0.0f, 0.0f);;

		size_t length = wcslen(text);

		for (uint64 i = 0; i < length; ++i)
		{
			wchar character = text[i];
			if (character == ' ')
				curPos.x += SpaceWidth();
			else if (character == '\n')
			{
				curPos.y += CharHeight();
				curPos.x = 0;
			}
			else
			{
				SpriteFont::CharDesc desc = GetCharDescriptor(character);
				curPos.x += desc.Width + 1;
			}

			textSize.x = std::max(curPos.x, textSize.x);
			textSize.y = std::max(curPos.y, textSize.y);
		}

		return size;
	}
}