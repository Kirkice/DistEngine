/* $Header: /common/Render/include/ArHosekSkyModel.h	  8/28/21 15:23pm Kirk 			    $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*						Project Name : DistEngine                                             *
*                                                                                             *
*						File Name : ArHosekSkyModel.h									      *
*                                                                                             *
*                       Programmer : Kirk                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#pragma once
#ifndef _ARHOSEK_SKYMODEL_H_
#define _ARHOSEK_SKYMODEL_H_

#ifdef __cplusplus
extern "C" {
#endif

    typedef double ArHosekSkyModelConfiguration[9];


    //   Spectral version of the model

    /* ----------------------------------------------------------------------------

        ArHosekSkyModelState struct
        ---------------------------

        This struct holds the pre-computation data for one particular albedo value.
        Most fields are self-explanatory, but users should never directly
        manipulate any of them anyway. The only consistent way to manipulate such
        structs is via the functions 'arhosekskymodelstate_alloc_init' and
        'arhosekskymodelstate_free'.

        'emission_correction_factor_sky'
        'emission_correction_factor_sun'

            The original model coefficients were fitted against the emission of
            our local sun. If a different solar emission is desired (i.e. if the
            model is being used to predict skydome appearance for an earth-like
            planet that orbits a different star), these correction factors, which
            are determined during the alloc_init step, are applied to each waveband
            separately (they default to 1.0 in normal usage). This is the simplest
            way to retrofit this sort of capability to the existing model. The
            different factors for sky and sun are needed since the solar disc may
            be of a different size compared to the terrestrial sun.

    ---------------------------------------------------------------------------- */

    typedef struct ArHosekSkyModelState
    {
        ArHosekSkyModelConfiguration  configs[11];
        double                        radiances[11];
        double                        turbidity;
        double                        solar_radius;
        double                        emission_correction_factor_sky[11];
        double                        emission_correction_factor_sun[11];
        double                        albedo;
        double                        elevation;
    }
    ArHosekSkyModelState;

    /* ----------------------------------------------------------------------------

        arhosekskymodelstate_alloc_init() function
        ------------------------------------------

        Initialises an ArHosekSkyModelState struct for a terrestrial setting.

    ---------------------------------------------------------------------------- */

    ArHosekSkyModelState* arhosekskymodelstate_alloc_init(
        const double  solar_elevation,
        const double  atmospheric_turbidity,
        const double  ground_albedo
    );


    /* ----------------------------------------------------------------------------

        arhosekskymodelstate_alienworld_alloc_init() function
        -----------------------------------------------------

        Initialises an ArHosekSkyModelState struct for an "alien world" setting
        with a sun of a surface temperature given in 'kelvin'. The parameter
        'solar_intensity' controls the overall brightness of the sky, relative
        to the solar irradiance on Earth. A value of 1.0 yields a sky dome that
        is, on average over the wavelenghts covered in the model (!), as bright
        as the terrestrial sky in radiometric terms.

        Which means that the solar radius has to be adjusted, since the
        emissivity of a solar surface with a given temperature is more or less
        fixed. So hotter suns have to be smaller to be equally bright as the
        terrestrial sun, while cooler suns have to be larger. Note that there are
        limits to the validity of the luminance patterns of the underlying model:
        see the discussion above for more on this. In particular, an alien sun with
        a surface temperature of only 2000 Kelvin has to be very large if it is
        to be as bright as the terrestrial sun - so large that the luminance
        patterns are no longer a really good fit in that case.

        If you need information about the solar radius that the model computes
        for a given temperature (say, for light source sampling purposes), you
        have to query the 'solar_radius' variable of the sky model state returned
        *after* running this function.

    ---------------------------------------------------------------------------- */

    ArHosekSkyModelState* arhosekskymodelstate_alienworld_alloc_init(
        const double  solar_elevation,
        const double  solar_intensity,
        const double  solar_surface_temperature_kelvin,
        const double  atmospheric_turbidity,
        const double  ground_albedo
    );

    void arhosekskymodelstate_free(
        ArHosekSkyModelState* state
    );

    double arhosekskymodel_radiance(
        ArHosekSkyModelState* state,
        double                  theta,
        double                  gamma,
        double                  wavelength
    );

    // CIE XYZ and RGB versions


    ArHosekSkyModelState* arhosek_xyz_skymodelstate_alloc_init(
        const double  turbidity,
        const double  albedo,
        const double  elevation
    );


    ArHosekSkyModelState* arhosek_rgb_skymodelstate_alloc_init(
        const double  turbidity,
        const double  albedo,
        const double  elevation
    );


    double arhosek_tristim_skymodel_radiance(
        ArHosekSkyModelState* state,
        double                  theta,
        double                  gamma,
        int                     channel
    );

    //   Delivers the complete function: sky + sun, including limb darkening.
    //   Please read the above description before using this - there are several
    //   caveats!

    double arhosekskymodel_solar_radiance(
        ArHosekSkyModelState* state,
        double                      theta,
        double                      gamma,
        double                      wavelength
    );

#ifdef __cplusplus
}
#endif

#endif // _ARHOSEK_SKYMODEL_H_
