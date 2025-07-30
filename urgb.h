/*
 * Copyright (C) 2025 therealblue24 under the MIT license.
 * ===========================================================
 *        ____   ____ ____
 *  _   _|  _ \ / ___| __ )      simple enough RGB-based
 * | | | | |_) | |  _|  _ \      colorspace where the
 * | |_| |  _ <| |_| | |_) |     length of the color
 *  \__,_|_| \_\\____|____/      is its lightness
 *
 * This code is stb-styled, define `URGB_IMPL` to generate
 * the implementation.
 *
 * uRGB is a colorspace that is basically like the RGB
 * colorspace except that the length of the color
 * ( sqrt(r^2 + g^2 + b^2) ) is the lightness of the
 * color.
 *
 * NOTE:   The gamut is different from the RGB gamut.
 * NOTE 2: Probably does not work for colors outside
           the RGB gamut (i.e. HDR)
 * NOTE 3: Good luck trying to make a Lab-like space
           out of this.
 */

#ifndef URGB_H_
#define URGB_H_

typedef struct RGB {
	float r, g, b;
} RGB;

typedef struct uRGB {
	float r, g, b;
} uRGB;

/* convert RGB -> uRGB */
uRGB rgb_to_urgb(RGB rgb);

/* convert uRGB -> RGB */
RGB urgb_to_rgb(uRGB urgb);

#endif /* URGB_H_ */

#ifdef __LSP__
#define URGB_IMPL
#endif /* __LSP__ */

#ifdef URGB_IMPL
#define URGB_IMPL

#include <math.h>
#include <float.h>

/* unused, here for refrence */
#define LUM_X (0.3194710612297058105468750f)
#define LUM_Y (0.4279969334602355957031250f)
#define LUM_Z (0.2525319755077362060546875f)

#define LUM_CBRT_X (0.683613307263600720129218866113f)
#define LUM_CBRT_Y (0.753610404504881907165394773691f)
#define LUM_CBRT_Z (0.632080111476343465946225018477f)

uRGB rgb_to_urgb(RGB rgb)
{
	/* normalize RGB space such that lightness is cbrt(r^3 + g^3 + b^3) */
	RGB normalized = rgb;
	normalized.r *= LUM_CBRT_X;
	normalized.g *= LUM_CBRT_Y;
	normalized.b *= LUM_CBRT_Z;

	/* now we need to transform the space such that the lightness is
     * L = sqrt(r^2 + g^2 + b^2) */

	/* first, compute lightness */
	float lightness = cbrtf(powf(normalized.r, 3.0) + powf(normalized.g, 3.0) +
							powf(normalized.b, 3.0));

	/* compute length of the normalized RGB color */
	float length = sqrtf(powf(normalized.r, 2.0) + powf(normalized.g, 2.0) +
						 powf(normalized.b, 2.0));

	/* to change length to lightness, simply find the ratio and multiply by that */
	float change = lightness / (length + FLT_MIN);

	return (uRGB){ change * normalized.r, change * normalized.g,
				   change * normalized.b };
}

RGB urgb_to_rgb(uRGB urgb)
{
	/* get lightness of the color (which is just the length) */
	float lightness =
		sqrtf(powf(urgb.r, 2.0) + powf(urgb.g, 2.0) + powf(urgb.b, 2.0));

	/* get the lightness using the original formula to change the space
	 * back into the normalized RGB */
	float lightness_conv =
		cbrtf(powf(urgb.r, 3.0) + powf(urgb.g, 3.0) + powf(urgb.b, 3.0));

	float change = lightness / (lightness_conv + FLT_MIN);

	/* blah blah same as last time */
	RGB normalized = { change * urgb.r, change * urgb.g, change * urgb.b };

	/* turn the normalized space back to normal RGB */

	return (RGB){ normalized.r / LUM_CBRT_X, normalized.g / LUM_CBRT_Y,
				  normalized.b / LUM_CBRT_Z };
}

#endif /* URGB_IMPL */
