//
// Created by chris on 09.04.21.
//

#ifndef DNGBINDINGS_WRAPPER_H
#define DNGBINDINGS_WRAPPER_H

typedef struct Area {
    unsigned short top;
    unsigned short left;
    unsigned short bottom;
    unsigned short right;
} Area;

typedef struct ImageInfoContainer {
    unsigned short width, height;
    const double black_levels[4], white_levels[4];
    const double camera_neutral[3];
    const float xyz_to_cam[3][3];
    Area active_area;
} ImageInfoContainer;

typedef struct ExifRational { int numerator; int denominator; } ExifRational;
typedef struct ExifURational { unsigned int numerator; unsigned int denominator; } ExifURational;

typedef struct ExifBindings {
    unsigned int (*get_uint)(void*, unsigned int, unsigned short, unsigned int*);
    unsigned int (*get_urational)(void*, unsigned int, unsigned short, ExifURational*);
    unsigned int (*get_rational)(void*, unsigned int, unsigned short, ExifRational*);
    char *  (*get_string)(void*, unsigned int);
    void (*free_rs_string)(char*);
} ExifBindings;


enum ExifTag {
    GPSInfo_GPSAltitude,
    GPSInfo_GPSAltitudeRef,
    GPSInfo_GPSAreaInformation,
    GPSInfo_GPSDateStamp,
    GPSInfo_GPSDestBearing,
    GPSInfo_GPSDestBearingRef,
    GPSInfo_GPSDestDistance,
    GPSInfo_GPSDestDistanceRef,
    GPSInfo_GPSDestLatitude,
    GPSInfo_GPSDestLatitudeRef,
    GPSInfo_GPSDestLongitude,
    GPSInfo_GPSDestLongitudeRef,
    GPSInfo_GPSDifferential,
    GPSInfo_GPSDOP,
    GPSInfo_GPSImgDirection,
    GPSInfo_GPSImgDirectionRef,
    GPSInfo_GPSLatitude,
    GPSInfo_GPSLatitudeRef,
    GPSInfo_GPSLongitude,
    GPSInfo_GPSLongitudeRef,
    GPSInfo_GPSMapDatum,
    GPSInfo_GPSMeasureMode,
    GPSInfo_GPSProcessingMethod,
    GPSInfo_GPSSatellites,
    GPSInfo_GPSSpeed,
    GPSInfo_GPSSpeedRef,
    GPSInfo_GPSStatus,
    GPSInfo_GPSTimeStamp,
    GPSInfo_GPSTrack,
    GPSInfo_GPSTrackRef,
    GPSInfo_GPSVersionID,
    Image_Artist,
    Image_Copyright,
    Image_DateTime,
    Image_ImageDescription,
    Image_Make,
    Image_Model,
    Image_Software,
    Iop_InteroperabilityIndex,
    Iop_InteroperabilityVersion,
    MakerNote_ByteOrder,
    MakerNote_Offset,
    Photo_ApertureValue,
    Photo_BodySerialNumber,
    Photo_BrightnessValue,
    Photo_CameraOwnerName,
    Photo_ColorSpace,
    Photo_ComponentsConfiguration,
    Photo_CompressedBitsPerPixel,
    Photo_Contrast,
    Photo_CustomRendered,
    Photo_DateTimeDigitized,
    Photo_DateTimeOriginal,
    Photo_DigitalZoomRatio,
    Photo_ExifVersion,
    Photo_ExposureBiasValue,
    Photo_ExposureIndex,
    Photo_ExposureMode,
    Photo_ExposureProgram,
    Photo_ExposureTime,
    Photo_FileSource,
    Photo_Flash,
    Photo_FNumber,
    Photo_FocalLength,
    Photo_FocalLengthIn35mmFilm,
    Photo_FocalPlaneResolutionUnit,
    Photo_FocalPlaneXResolution,
    Photo_FocalPlaneYResolution,
    Photo_GainControl,
    Photo_Gamma,
    Photo_ISOSpeed,
    Photo_ISOSpeedLatitudeyyy,
    Photo_ISOSpeedLatitudezzz,
    Photo_ISOSpeedRatings,
    Photo_LensMake,
    Photo_LensModel,
    Photo_LensSerialNumber,
    Photo_LensSpecification,
    Photo_LightSource,
    Photo_MakerNote,
    Photo_MaxApertureValue,
    Photo_MeteringMode,
    Photo_PixelXDimension,
    Photo_PixelYDimension,
    Photo_RecommendedExposureIndex,
    Photo_Saturation,
    Photo_SceneCaptureType,
    Photo_SceneType,
    Photo_SensingMethod,
    Photo_SensitivityType,
    Photo_Sharpness,
    Photo_ShutterSpeedValue,
    Photo_StandardOutputSensitivity,
    Photo_SubjectArea,
    Photo_SubjectDistance,
    Photo_SubjectDistanceRange,
    Photo_UserComment,
    Photo_WhiteBalance
};

#endif //DNGBINDINGS_WRAPPER_H
