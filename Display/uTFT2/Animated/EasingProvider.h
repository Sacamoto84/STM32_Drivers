//https://github.com/MasayukiSuda/EasingInterpolator

#ifndef EASINGPROVIDER_H_
#define EASINGPROVIDER_H_

#include "math.h"      /* pow */

enum Ease {
    LINEAR,
    QUAD_IN,
    QUAD_OUT,
    QUAD_IN_OUT,
    CUBIC_IN,
    CUBIC_OUT,
    CUBIC_IN_OUT,
    QUART_IN,
    QUART_OUT,
    QUART_IN_OUT,
    QUINT_IN,
    QUINT_OUT,
    QUINT_IN_OUT,
    SINE_IN,
    SINE_OUT,
    SINE_IN_OUT,
    BACK_IN,
    BACK_OUT,
    BACK_IN_OUT,
    CIRC_IN,
    CIRC_OUT,
    CIRC_IN_OUT,
    BOUNCE_IN,
    BOUNCE_OUT,
    BOUNCE_IN_OUT,
    ELASTIC_IN,
    ELASTIC_OUT,
    ELASTIC_IN_OUT,
    EASE_IN_EXPO,
    EASE_OUT_EXPO,
    EASE_IN_OUT_EXPO
};

#ifndef PI
#define PI 3.1415926535897932384626433832795
#endif

class EasingProvider {

private:

    static inline float pow_n(float x, int n)
    {
        float res = x;
        for (int i = 1; i < n; i++) res *= x;
        return res;
    }

    float getPowIn(float elapsedTimeRate, int p)
    {
        return pow_n(elapsedTimeRate, p);
    }

    float getPowOut(float elapsedTimeRate, int p)
    {
        return 1.0F - pow_n(1.0F - elapsedTimeRate, p);
    }

    float getPowInOut(float elapsedTimeRate, int p) {
        float e = elapsedTimeRate * 2.0F;
        if (e < 1.0F) {
            return 0.5F * pow_n(e, p);
        }

        return 1.0F - 0.5F * pow_n(2.0F - e, p);
    }

    float getBackInOut(float elapsedTimeRate, float amount) {
       amount *= 1.525F;
       float e = elapsedTimeRate * 2.0F;
       if (e < 1.0F) {
           return 0.5F * (e * e * ((amount + 1.0F) * e - amount));
       }
       e = elapsedTimeRate * 2.0F - 2.0F;
       return 0.5F * (e * e * ((amount + 1.0F) * e + amount) + 2.0F);
   }

   float getBounceIn(float elapsedTimeRate)
   {
	   return 1.0F - getBounceOut(1.0F - elapsedTimeRate);
   }

   float getBounceOut(float elapsedTimeRate) {
       if (elapsedTimeRate < 1.0F / 2.75F) {
           return 7.5625F * elapsedTimeRate * elapsedTimeRate;
        } else if (elapsedTimeRate < 2.0F / 2.75F) {
     	   float e = elapsedTimeRate - 1.5F / 2.75F;
            return 7.5625F * e * e + 0.75F;
        } else if (elapsedTimeRate < 2.5F / 2.75F) {
     	   float e = elapsedTimeRate - 2.25F / 2.75F;
            return 7.5625F * e * e + 0.9375F;
        } else {
     	   float e = elapsedTimeRate - 2.625F / 2.75F;
            return 7.5625F * e * e + 0.984375F;
        }
   }

   float getElasticIn(float elapsedTimeRate, double amplitude, double period) {
       if (elapsedTimeRate == 0 || elapsedTimeRate == 1) return elapsedTimeRate;
       double pi2 = PI * 2;
       double s = period / pi2 * asin(1.0F / amplitude);
       float e = elapsedTimeRate - 1.0F;
       return (float) -(amplitude * pow(2.0F, 10.0F * e) * sin((elapsedTimeRate - s) * pi2 / period));
   }

   float getElasticOut(float elapsedTimeRate, double amplitude, double period) {
       if (elapsedTimeRate == 0 || elapsedTimeRate == 1) return elapsedTimeRate;

       double pi2 = PI * 2;
       double s = period / pi2 * asin(1 / amplitude);
       return (float) (amplitude * pow(2, -10 * elapsedTimeRate) * sin((elapsedTimeRate - s) * pi2 / period) + 1);
   }

   float getElasticInOut(float elapsedTimeRate, double amplitude, double period) {
       double pi2 = PI * 2;
       double s = period / pi2 * asin(1.0F / amplitude);

       float e = elapsedTimeRate * 2.0F;
       if (e < 1.0F) {
           float e2 = e - 1.0F;
           return (float) (-0.5F * (amplitude * pow(2, 10 * e2) * sin((elapsedTimeRate - s) * pi2 / period)));
       }

       e = e - 1.0F;
       return (float) (amplitude * pow(2, -10 * e) * sin((elapsedTimeRate - s) * pi2 / period) * 0.5 + 1.0);
   }
	/**
	     * @param ease            Easing type
	     * @param elapsedTimeRate Elapsed time / Total time
	     * @return easedValue
	     */




public:

   float get(Ease ease, float elapsedTimeRate) {
	   float e;
       switch (ease) {
           case LINEAR:
               return elapsedTimeRate;
           case QUAD_IN:
               return getPowIn(elapsedTimeRate, 2);
           case QUAD_OUT:
               return getPowOut(elapsedTimeRate, 2);
           case QUAD_IN_OUT:
               return getPowInOut(elapsedTimeRate, 2);
           case CUBIC_IN:
               return getPowIn(elapsedTimeRate, 3);
           case CUBIC_OUT:
               return getPowOut(elapsedTimeRate, 3);
           case CUBIC_IN_OUT:
               return getPowInOut(elapsedTimeRate, 3);
           case QUART_IN:
               return getPowIn(elapsedTimeRate, 4);
           case QUART_OUT:
               return getPowOut(elapsedTimeRate, 4);
           case QUART_IN_OUT:
               return getPowInOut(elapsedTimeRate, 4);
           case QUINT_IN:
               return getPowIn(elapsedTimeRate, 5);
           case QUINT_OUT:
               return getPowOut(elapsedTimeRate, 5);
           case QUINT_IN_OUT:
               return getPowInOut(elapsedTimeRate, 5);
           case SINE_IN:
               return (float) (1.0F - cos(elapsedTimeRate * PI / 2.0F));
           case SINE_OUT:
               return (float) sin(elapsedTimeRate * PI / 2.0F);
           case SINE_IN_OUT:
               return (float) (-0.5f * (cos(PI * elapsedTimeRate) - 1.0F));
           case BACK_IN:
               return (float) (  elapsedTimeRate * elapsedTimeRate * ((1.7 + 1.0F) * elapsedTimeRate - 1.7));
            case BACK_OUT:
            {   e = elapsedTimeRate - 1;
                return (float) (e * e * ((1.7 + 1.0F) * e + 1.7) + 1.0F);}
           case BACK_IN_OUT:
               return getBackInOut(elapsedTimeRate, 1.7f);
           case CIRC_IN:
               return (float) -(sqrt(1.0F - elapsedTimeRate * elapsedTimeRate) - 1);

            case CIRC_OUT:{
         	   e = elapsedTimeRate - 1;
                return (float) sqrt(1.0F - e * e);}

            case CIRC_IN_OUT:
                e = elapsedTimeRate * 2.0F;
                if ( e < 1.0F) {
                    return (float) (-0.5f * (sqrt(1.0F - e * e) - 1.0F));
                }
                e = elapsedTimeRate * 2.0F - 2.0F;
                return (float) (0.5f * (sqrt(1.0F - e * e) + 1.0F));
           case BOUNCE_IN:
               return getBounceIn(elapsedTimeRate);
           case BOUNCE_OUT:
               return getBounceOut(elapsedTimeRate);
           case BOUNCE_IN_OUT:
               if (elapsedTimeRate < 0.5f) {
                   return getBounceIn(elapsedTimeRate * 2.0F) * 0.5F;
               }
               return getBounceOut(elapsedTimeRate * 2.0F - 1.0F) * 0.5F + 0.5F;
           case ELASTIC_IN:
               return getElasticIn(elapsedTimeRate, 1, 0.3);

           case ELASTIC_OUT:
               return getElasticOut(elapsedTimeRate, 1, 0.3);

           case ELASTIC_IN_OUT:
               return getElasticInOut(elapsedTimeRate, 1, 0.45);

           case EASE_IN_EXPO: {
               return (float) pow(2, 10 * (elapsedTimeRate - 1));
           }
           case EASE_OUT_EXPO: {
               return (float) -pow(2, -10 * elapsedTimeRate) + 1;
           }
            case EASE_IN_OUT_EXPO: {
                e = elapsedTimeRate * 2;
                if (e < 1) {
                    return (float) pow(2, 10 * (e - 1.0F)) * 0.5f;
                }
                e -= 1.0F;
                return (float) (-pow(2, -10 * e) + 2.0F) * 0.5F;
            }
           default:
               return elapsedTimeRate;

       }






















	    }








};

#endif /* EASINGPROVIDER_H_ */
