/* Copyright (C) 2015 Fimagena

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.

   This file uses code from dngconvert from Jens Mueller and others
   (https://github.com/jmue/dngconvert) - Copyright (C) 2011 Jens 
   Mueller (tschensinger at gmx dot de)
*/

#include "negativeProcessor.h"
// #include "vendorProcessors/DNGprocessor.h"
// #include "vendorProcessors/ILCE7processor.h"
// #include "vendorProcessors/FujiProcessor.h"
#include "vendorProcessors/variousVendorProcessor.h"

#include <stdexcept>
#include <iostream>

#include <dng_simple_image.h>
#include <dng_camera_profile.h>
#include <dng_file_stream.h>
#include <dng_memory_stream.h>
#include <dng_xmp.h>

#include <zlib.h>

/*#include <exiv2/error.hpp>
#include <exiv2/image.hpp>
#include <exiv2/xmp_exiv2.hpp>
#include <libraw/libraw.h>*/

const char* getDngErrorMessage(int errorCode) {
    switch (errorCode) {
        default:
        case 100000: return "Unknown error";
        case 100003: return "Processing stopped by user (or host application) request";
        case 100004: return "Necessary host functionality is not present";
        case 100005: return "Out of memory";
        case 100006: return "File format is not valid";
        case 100007: return "Matrix has wrong shape, is badly conditioned, or similar problem";
        case 100008: return "Could not open file";
        case 100009: return "Error reading file";
        case 100010: return "Error writing file";
        case 100011: return "Unexpected end of file";
        case 100012: return "File is damaged in some way";
        case 100013: return "Image is too big to save as DNG";
        case 100014: return "Image is too big to save as TIFF";
        case 100015: return "DNG version is unsupported";
    }
}


NegativeProcessor * NegativeProcessor::createProcessor(AutoPtr<dng_host> &host, ImageInfoContainer image_info,
                                                       ExifBindings exif_bindings, void *exif_context,
                                                       const char *make, const char *model) {
    // -----------------------------------------------------------------------------------------
    // Open and parse rawfile with libraw...

    /*AutoPtr<LibRaw> rawProcessor(new LibRaw());

    int ret = rawProcessor->open_file(filename);
    if (ret != LIBRAW_SUCCESS) {
        rawProcessor->recycle();
        std::stringstream error; error << "LibRaw-error while opening rawFile: " << libraw_strerror(ret);
        throw std::runtime_error(error.str());
    }

    ret = rawProcessor->unpack();
    if (ret != LIBRAW_SUCCESS) {
        rawProcessor->recycle();
        std::stringstream error; error << "LibRaw-error while unpacking rawFile: " << libraw_strerror(ret);
        throw std::runtime_error(error.str());
    }*/

    // -----------------------------------------------------------------------------------------
    // ...and libexiv2

    /*Exiv2::Image::AutoPtr rawImage;
    try {
        rawImage = Exiv2::ImageFactory::open(filename);
        rawImage->readMetadata();
    }
    catch (Exiv2::Error& e) {
        std::stringstream error; error << "Exiv2-error while opening/parsing rawFile (code " << e.code() << "): " << e.what();
        throw std::runtime_error(error.str());
    }*/

    // -----------------------------------------------------------------------------------------
    // Identify and create correct processor class

//    if (rawProcessor->imgdata.idata.dng_version != 0) {
//        try {return new DNGprocessor(host, rawProcessor.Release(), rawImage);}
//        catch (dng_exception &e) {
//            std::stringstream error; error << "Cannot parse source DNG-file (" << e.ErrorCode() << ": " << getDngErrorMessage(e.ErrorCode()) << ")";
//            throw std::runtime_error(error.str());
//        }
//    }
//    else if (!strcmp(rawProcessor->imgdata.idata.model, "ILCE-7"))
//        return new ILCE7processor(host, rawProcessor.Release(), rawImage);
//    else if (!strcmp(rawProcessor->imgdata.idata.make, "FUJIFILM"))
//        return new FujiProcessor(host, rawProcessor.Release(), rawImage);

    return new VariousVendorProcessor(host, image_info, exif_bindings, exif_context, make, model);
}


NegativeProcessor::NegativeProcessor(AutoPtr<dng_host> &host, ImageInfoContainer image_info, ExifBindings exif_bindings,
                                     void *exif_context, const char *make, const char *model)
                                   : m_host(host), image_width(image_info.width), image_height(image_info.height), rs_image(image_info), exif_bindings(exif_bindings), exif_context(exif_context), make(make), model(model) {
    m_negative.Reset(m_host->Make_dng_negative());
}


NegativeProcessor::~NegativeProcessor() {
	//m_RawProcessor->recycle();
}


ColorKeyCode colorKey(const char color) {
    switch (color) {
        case 'R': return colorKeyRed;
        case 'G': return colorKeyGreen;
        case 'B': return colorKeyBlue;
        case 'C': return colorKeyCyan;
        case 'M': return colorKeyMagenta;
        case 'Y': return colorKeyYellow;
        case 'E': return colorKeyCyan; // only in the Sony DSC-F828. 'Emerald' - like cyan according to Sony
        case 'T':                      // what is 'T'??? LibRaw reports that only for the Leaf Catchlight, so I guess we're not compatible with early '90s tech...
        default:  return colorKeyMaxEnum;
    }
}


void NegativeProcessor::setDNGPropertiesFromRaw() {
	// -----------------------------------------------------------------------------------------
	// Model

    dng_string makeModel;
    makeModel.Append(make.c_str());
    makeModel.Append(" ");
    makeModel.Append(model.c_str());
    m_negative->SetModelName(makeModel.Get());

    // -----------------------------------------------------------------------------------------
    // Orientation

    m_negative->SetBaseOrientation(dng_orientation::Normal());

	// -----------------------------------------------------------------------------------------
	// ColorKeys (this needs to happen before Mosaic - how about documenting that in the SDK???)

    m_negative->SetColorChannels(3);
    m_negative->SetColorKeys(colorKeyRed, colorKeyGreen, colorKeyBlue);

    // -----------------------------------------------------------------------------------------
    // Mosaic

    m_negative->SetBayerMosaic(1);

	// -----------------------------------------------------------------------------------------
	// Default scale and crop/active area

    m_negative->SetDefaultScale(dng_urational(1, 1), dng_urational(1, 1));
    m_negative->SetActiveArea(dng_rect(
                rs_image.active_area.top,
                rs_image.active_area.left,
                rs_image.active_area.bottom,
                rs_image.active_area.right
            ));

    uint32 cropWidth = rs_image.active_area.right - rs_image.active_area.left - 16;
    uint32 cropHeight = rs_image.active_area.bottom - rs_image.active_area.top - 16;

    m_negative->SetDefaultCropOrigin(8, 8);
    m_negative->SetDefaultCropSize(cropWidth, cropHeight);

    // New
    //m_negative->SetMaskedArea(dng_rect(0, 0, 3708, 68));

    // -----------------------------------------------------------------------------------------
    // CameraNeutral

    //FIXME: what does this actually do?
    //FIXME: some pictures have 0 in cam_mul leading to NaNs; corrected here with 0-override but is that correct?
    dng_vector cameraNeutral(3);

    for (int i = 0; i < 4; i++)
        cameraNeutral[i] = rs_image.camera_neutral[i];

    m_negative->SetCameraNeutral(cameraNeutral);

    // -----------------------------------------------------------------------------------------
    // BlackLevel & WhiteLevel

    for (int i = 0; i < 4; i++)
        m_negative->SetWhiteLevel(rs_image.white_levels[i], i);

    for (int i = 0; i < 4; i++)
        m_negative->SetBlackLevel(rs_image.black_levels[i], i);


    // -----------------------------------------------------------------------------------------
    // Fixed properties

    m_negative->SetBaselineExposure(0.0);                       // should be fixed per camera
    m_negative->SetBaselineNoise(1.0);
    m_negative->SetBaselineSharpness(1.0);

    // default
    m_negative->SetAntiAliasStrength(dng_urational(100, 100));  // = no aliasing artifacts
    m_negative->SetLinearResponseLimit(1.0);                    // = no non-linear sensor response
    m_negative->SetAnalogBalance(dng_vector_3(1.0, 1.0, 1.0));
    m_negative->SetShadowScale(dng_urational(1, 1));
}


void NegativeProcessor::setCameraProfile(const char *dcpFilename) {
    AutoPtr<dng_camera_profile> prof(new dng_camera_profile);

    if (strlen(dcpFilename) > 0) {
        dng_file_stream profStream(dcpFilename);
        if (!prof->ParseExtended(profStream))
            throw std::runtime_error("Could not parse supplied camera profile file!");
    }
    else {
        // -----------------------------------------------------------------------------------------
        // Build our own minimal profile, based on one colour matrix provided by LibRaw

        dng_string profName;
        profName.Append(make.c_str());
        profName.Append(" ");
        profName.Append(model.c_str());

        prof->SetName(profName.Get());
        prof->SetCalibrationIlluminant1(lsD65);


        auto *colormatrix1 = new dng_matrix(3, 3);
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                (*colormatrix1)[i][j] = rs_image.xyz_to_cam[i][j];
        prof->SetColorMatrix1(*colormatrix1);
        prof->SetProfileCalibrationSignature("de.croaxeldyphic.dngrs");
    }

    m_negative->AddProfile(prof);
}


void NegativeProcessor::setExifFromRaw(const dng_date_time_info &dateTimeNow, const dng_string &appNameVersion) {
    dng_exif *negExif = m_negative->GetExif();

    // -----------------------------------------------------------------------------------------
    // TIFF 6.0 "D. Other Tags"
    getRawExifTag(ExifTag::Image_DateTime, &negExif->fDateTime);
    getRawExifTag(ExifTag::Image_ImageDescription, &negExif->fImageDescription);
    getRawExifTag(ExifTag::Image_Make, &negExif->fMake);
    getRawExifTag(ExifTag::Image_Model, &negExif->fModel);
    getRawExifTag(ExifTag::Image_Software, &negExif->fSoftware);
    getRawExifTag(ExifTag::Image_Artist, &negExif->fArtist);
    getRawExifTag(ExifTag::Image_Copyright, &negExif->fCopyright);

    // -----------------------------------------------------------------------------------------
    // Exif 2.3 "A. Tags Relating to Version" (order as in spec)
    getRawExifTag(ExifTag::Photo_ExifVersion, 0, &negExif->fExifVersion);
    // Exif.Photo.FlashpixVersion - fFlashPixVersion : ignoring this here

    // -----------------------------------------------------------------------------------------
    // Exif 2.3 "B. Tags Relating to Image data Characteristics" (order as in spec)
    getRawExifTag(ExifTag::Photo_ColorSpace, 0, &negExif->fColorSpace);
    // Gamma : Supported by DNG SDK (fGamma) but not Exiv2 (v0.24)
    getRawExifTag(ExifTag::Photo_Gamma, 0, &negExif->fGamma);

    // -----------------------------------------------------------------------------------------
    // Exif 2.3 "C. Tags Relating To Image Configuration" (order as in spec)
    getRawExifTag(ExifTag::Photo_ComponentsConfiguration, 0, &negExif->fComponentsConfiguration);
    getRawExifTag(ExifTag::Photo_CompressedBitsPerPixel, 0, &negExif->fCompresssedBitsPerPixel);  // nice typo in DNG SDK...
    getRawExifTag(ExifTag::Photo_PixelXDimension, 0, &negExif->fPixelXDimension);
    getRawExifTag(ExifTag::Photo_PixelYDimension, 0, &negExif->fPixelYDimension);

    // -----------------------------------------------------------------------------------------
    // Exif 2.3 "D. Tags Relating to User Information" (order as in spec)
    // MakerNote: We'll deal with that below
    getRawExifTag(ExifTag::Photo_UserComment, &negExif->fUserComment);

    // -----------------------------------------------------------------------------------------
    // Exif 2.3 "E. Tags Relating to Related File Information" (order as in spec)
    // RelatedSoundFile : DNG SDK doesn't support this

    // -----------------------------------------------------------------------------------------
    // Exif 2.3 "F. Tags Relating to Date and Time" (order as in spec)
    getRawExifTag(ExifTag::Photo_DateTimeOriginal, &negExif->fDateTimeOriginal);
    getRawExifTag(ExifTag::Photo_DateTimeDigitized, &negExif->fDateTimeDigitized);
    // SubSecTime          : DNG SDK doesn't support this
    // SubSecTimeOriginal  : DNG SDK doesn't support this
    // SubSecTimeDigitized : DNG SDK doesn't support this

    // -----------------------------------------------------------------------------------------
    // Exif 2.3 "G. Tags Relating to Picture-Taking Conditions" (order as in spec)
    getRawExifTag(ExifTag::Photo_ExposureTime, 0, &negExif->fExposureTime);
    getRawExifTag(ExifTag::Photo_FNumber, 0, &negExif->fFNumber);
    getRawExifTag(ExifTag::Photo_ExposureProgram, 0, &negExif->fExposureProgram);
    // SpectralSensitivity : DNG SDK doesn't support this
    getRawExifTag(ExifTag::Photo_ISOSpeedRatings, negExif->fISOSpeedRatings, 3); // PhotographicSensitivity in Exif 2.3
    // OECF : DNG SDK doesn't support this
    getRawExifTag(ExifTag::Photo_SensitivityType, 0, &negExif->fSensitivityType);
    getRawExifTag(ExifTag::Photo_StandardOutputSensitivity, 0, &negExif->fStandardOutputSensitivity);
    getRawExifTag(ExifTag::Photo_RecommendedExposureIndex, 0, &negExif->fRecommendedExposureIndex);
    getRawExifTag(ExifTag::Photo_ISOSpeed, 0, &negExif->fISOSpeed);
    getRawExifTag(ExifTag::Photo_ISOSpeedLatitudeyyy, 0, &negExif->fISOSpeedLatitudeyyy);
    getRawExifTag(ExifTag::Photo_ISOSpeedLatitudezzz, 0, &negExif->fISOSpeedLatitudezzz);
    getRawExifTag(ExifTag::Photo_ShutterSpeedValue, 0, &negExif->fShutterSpeedValue);
    getRawExifTag(ExifTag::Photo_ApertureValue, 0, &negExif->fApertureValue);
    getRawExifTag(ExifTag::Photo_BrightnessValue, 0, &negExif->fBrightnessValue);
    getRawExifTag(ExifTag::Photo_ExposureBiasValue, 0, &negExif->fExposureBiasValue);
    getRawExifTag(ExifTag::Photo_MaxApertureValue, 0, &negExif->fMaxApertureValue);
    getRawExifTag(ExifTag::Photo_SubjectDistance, 0, &negExif->fSubjectDistance);
    getRawExifTag(ExifTag::Photo_MeteringMode, 0, &negExif->fMeteringMode);
    getRawExifTag(ExifTag::Photo_LightSource, 0, &negExif->fLightSource);
    getRawExifTag(ExifTag::Photo_Flash, 0, &negExif->fFlash);
    getRawExifTag(ExifTag::Photo_FocalLength, 0, &negExif->fFocalLength);
    negExif->fSubjectAreaCount = getRawExifTag(ExifTag::Photo_SubjectArea, negExif->fSubjectArea, 4);
    // FlashEnergy : DNG SDK doesn't support this
    // SpatialFrequencyResponse : DNG SDK doesn't support this
    getRawExifTag(ExifTag::Photo_FocalPlaneXResolution, 0, &negExif->fFocalPlaneXResolution);
    getRawExifTag(ExifTag::Photo_FocalPlaneYResolution, 0, &negExif->fFocalPlaneYResolution);
    getRawExifTag(ExifTag::Photo_FocalPlaneResolutionUnit, 0, &negExif->fFocalPlaneResolutionUnit);
    // SubjectLocation : DNG SDK doesn't support this
    getRawExifTag(ExifTag::Photo_ExposureIndex, 0, &negExif->fExposureIndex);
    getRawExifTag(ExifTag::Photo_SensingMethod, 0, &negExif->fSensingMethod);
    getRawExifTag(ExifTag::Photo_FileSource, 0, &negExif->fFileSource);
    getRawExifTag(ExifTag::Photo_SceneType, 0, &negExif->fSceneType);
    // CFAPattern: we write it manually from raw data further below
    getRawExifTag(ExifTag::Photo_CustomRendered, 0, &negExif->fCustomRendered);
    getRawExifTag(ExifTag::Photo_ExposureMode, 0, &negExif->fExposureMode);
    getRawExifTag(ExifTag::Photo_WhiteBalance, 0, &negExif->fWhiteBalance);
    getRawExifTag(ExifTag::Photo_DigitalZoomRatio, 0, &negExif->fDigitalZoomRatio);
    getRawExifTag(ExifTag::Photo_FocalLengthIn35mmFilm, 0, &negExif->fFocalLengthIn35mmFilm);
    getRawExifTag(ExifTag::Photo_SceneCaptureType, 0, &negExif->fSceneCaptureType);
    getRawExifTag(ExifTag::Photo_GainControl, 0, &negExif->fGainControl);
    getRawExifTag(ExifTag::Photo_Contrast, 0, &negExif->fContrast);
    getRawExifTag(ExifTag::Photo_Saturation, 0, &negExif->fSaturation);
    getRawExifTag(ExifTag::Photo_Sharpness, 0, &negExif->fSharpness);
    // DeviceSettingsDescription : DNG SDK doesn't support this
    getRawExifTag(ExifTag::Photo_SubjectDistanceRange, 0, &negExif->fSubjectDistanceRange);

    // -----------------------------------------------------------------------------------------
    // Exif 2.3 "H. Other Tags" (order as in spec)
    // ImageUniqueID : DNG SDK doesn't support this
    getRawExifTag(ExifTag::Photo_CameraOwnerName, &negExif->fOwnerName);
    getRawExifTag(ExifTag::Photo_BodySerialNumber, &negExif->fCameraSerialNumber);
    getRawExifTag(ExifTag::Photo_LensSpecification, negExif->fLensInfo, 4);
    getRawExifTag(ExifTag::Photo_LensMake, &negExif->fLensMake);
    getRawExifTag(ExifTag::Photo_LensModel, &negExif->fLensName);
    getRawExifTag(ExifTag::Photo_LensSerialNumber, &negExif->fLensSerialNumber);

    // -----------------------------------------------------------------------------------------
    // Exif 2.3 GPS "A. Tags Relating to GPS" (order as in spec)
    uint32 gpsVer[4];  gpsVer[0] = gpsVer[1] = gpsVer[2] = gpsVer[3] = 0;
    getRawExifTag(ExifTag::GPSInfo_GPSVersionID, gpsVer, 4);
    negExif->fGPSVersionID = (gpsVer[0] << 24) + (gpsVer[1] << 16) + (gpsVer[2] <<  8) + gpsVer[3];
    getRawExifTag(ExifTag::GPSInfo_GPSLatitudeRef, &negExif->fGPSLatitudeRef);
    getRawExifTag(ExifTag::GPSInfo_GPSLatitude, negExif->fGPSLatitude, 3);
    getRawExifTag(ExifTag::GPSInfo_GPSLongitudeRef, &negExif->fGPSLongitudeRef);
    getRawExifTag(ExifTag::GPSInfo_GPSLongitude, negExif->fGPSLongitude, 3);
    getRawExifTag(ExifTag::GPSInfo_GPSAltitudeRef, 0, &negExif->fGPSAltitudeRef);
    getRawExifTag(ExifTag::GPSInfo_GPSAltitude, 0, &negExif->fGPSAltitude);
    getRawExifTag(ExifTag::GPSInfo_GPSTimeStamp, negExif->fGPSTimeStamp, 3);
    getRawExifTag(ExifTag::GPSInfo_GPSSatellites, &negExif->fGPSSatellites);
    getRawExifTag(ExifTag::GPSInfo_GPSStatus, &negExif->fGPSStatus);
    getRawExifTag(ExifTag::GPSInfo_GPSMeasureMode, &negExif->fGPSMeasureMode);
    getRawExifTag(ExifTag::GPSInfo_GPSDOP, 0, &negExif->fGPSDOP);
    getRawExifTag(ExifTag::GPSInfo_GPSSpeedRef, &negExif->fGPSSpeedRef);
    getRawExifTag(ExifTag::GPSInfo_GPSSpeed, 0, &negExif->fGPSSpeed);
    getRawExifTag(ExifTag::GPSInfo_GPSTrackRef, &negExif->fGPSTrackRef);
    getRawExifTag(ExifTag::GPSInfo_GPSTrack, 0, &negExif->fGPSTrack);
    getRawExifTag(ExifTag::GPSInfo_GPSImgDirectionRef, &negExif->fGPSImgDirectionRef);
    getRawExifTag(ExifTag::GPSInfo_GPSImgDirection, 0, &negExif->fGPSImgDirection);
    getRawExifTag(ExifTag::GPSInfo_GPSMapDatum, &negExif->fGPSMapDatum);
    getRawExifTag(ExifTag::GPSInfo_GPSDestLatitudeRef, &negExif->fGPSDestLatitudeRef);
    getRawExifTag(ExifTag::GPSInfo_GPSDestLatitude, negExif->fGPSDestLatitude, 3);
    getRawExifTag(ExifTag::GPSInfo_GPSDestLongitudeRef, &negExif->fGPSDestLongitudeRef);
    getRawExifTag(ExifTag::GPSInfo_GPSDestLongitude, negExif->fGPSDestLongitude, 3);
    getRawExifTag(ExifTag::GPSInfo_GPSDestBearingRef, &negExif->fGPSDestBearingRef);
    getRawExifTag(ExifTag::GPSInfo_GPSDestBearing, 0, &negExif->fGPSDestBearing);
    getRawExifTag(ExifTag::GPSInfo_GPSDestDistanceRef, &negExif->fGPSDestDistanceRef);
    getRawExifTag(ExifTag::GPSInfo_GPSDestDistance, 0, &negExif->fGPSDestDistance);
    getRawExifTag(ExifTag::GPSInfo_GPSProcessingMethod, &negExif->fGPSProcessingMethod);
    getRawExifTag(ExifTag::GPSInfo_GPSAreaInformation, &negExif->fGPSAreaInformation);
    getRawExifTag(ExifTag::GPSInfo_GPSDateStamp, &negExif->fGPSDateStamp);
    getRawExifTag(ExifTag::GPSInfo_GPSDifferential, 0, &negExif->fGPSDifferential);
    // GPSHPositioningError : Supported by DNG SDK (fGPSHPositioningError) but not Exiv2 (v0.24)

    // -----------------------------------------------------------------------------------------
    // Exif 2.3, Interoperability IFD "A. Attached Information Related to Interoperability"
    getRawExifTag(ExifTag::Iop_InteroperabilityIndex, &negExif->fInteroperabilityIndex);
    getRawExifTag(ExifTag::Iop_InteroperabilityVersion, 0, &negExif->fInteroperabilityVersion); // this is not in the Exif standard but in DNG SDK and Exiv2

/*  Fields in the DNG SDK Exif structure that we are ignoring here. Some could potentially be 
    read through Exiv2 but are not part of the Exif standard so we leave them out:
     - fBatteryLevelA, fBatteryLevelR
     - fSelfTimerMode
     - fTIFF_EP_StandardID, fImageNumber
     - fApproxFocusDistance
     - fFlashCompensation, fFlashMask
     - fFirmware 
     - fLensID
     - fLensNameWasReadFromExif (--> available but not used by SDK)
     - fRelatedImageFileFormat, fRelatedImageWidth, fRelatedImageLength */

    // -----------------------------------------------------------------------------------------
    // Write CFAPattern (section G) manually from mosaicinfo

    const dng_mosaic_info* mosaicinfo = m_negative->GetMosaicInfo();
    if (mosaicinfo != NULL) {
        negExif->fCFARepeatPatternCols = mosaicinfo->fCFAPatternSize.v;
        negExif->fCFARepeatPatternRows = mosaicinfo->fCFAPatternSize.h;
        for (uint16 c = 0; c < negExif->fCFARepeatPatternCols; c++)
            for (uint16 r = 0; r < negExif->fCFARepeatPatternRows; r++)
                negExif->fCFAPattern[r][c] = mosaicinfo->fCFAPattern[c][r];
    }

    // -----------------------------------------------------------------------------------------
    // Reconstruct LensName from LensInfo if not present

    if (negExif->fLensName.IsEmpty()) {
        dng_urational *li = negExif->fLensInfo;
        std::stringstream lensName; lensName.precision(1); lensName.setf(std::ios::fixed, std::ios::floatfield);

        if (li[0].IsValid())      lensName << li[0].As_real64();
        if (li[1] != li[2])       lensName << "-" << li[1].As_real64();
        if (lensName.tellp() > 0) lensName << " mm";
        if (li[2].IsValid())      lensName << " f/" << li[2].As_real64();
        if (li[3] != li[2])       lensName << "-" << li[3].As_real64();

        negExif->fLensName.Set_ASCII(lensName.str().c_str());
    }

    // -----------------------------------------------------------------------------------------
    // Overwrite date, software, version - these are now referencing the DNG-creation

    negExif->fDateTime = dateTimeNow;
    negExif->fSoftware = appNameVersion;
    negExif->fExifVersion = DNG_CHAR4 ('0','2','3','0'); 
}


void NegativeProcessor::setXmpFromRaw(const dng_date_time_info &dateTimeNow, const dng_string &appNameVersion) {
    // -----------------------------------------------------------------------------------------
    // Copy existing XMP-tags in raw-file to DNG

    AutoPtr<dng_xmp> negXmp(new dng_xmp(m_host->Allocator()));
    /*for (Exiv2::XmpData::const_iterator it = m_RawXmp.begin(); it != m_RawXmp.end(); it++) {
        try {
            negXmp->Set(Exiv2::XmpProperties::nsInfo(it->groupName())->ns_, it->tagName().c_str(), it->toString().c_str());
        }
        catch (dng_exception& e) {
            // the above will throw an exception when trying to add XMPs with unregistered (i.e., unknown) 
            // namespaces -- we just drop them here.
            std::cerr << "Dropped XMP-entry from raw-file since namespace is unknown: "
                         "NS: "   << Exiv2::XmpProperties::nsInfo(it->groupName())->ns_ << ", "
                         "path: " << it->tagName().c_str() << ", "
                         "text: " << it->toString().c_str() << "\n";
        }
    }*/

    // -----------------------------------------------------------------------------------------
    // Set some base-XMP tags (incl. redundant creation date under Photoshop namespace - just to stay close to Adobe...)

    negXmp->UpdateDateTime(dateTimeNow);
    negXmp->UpdateMetadataDate(dateTimeNow);
    negXmp->SetString(XMP_NS_XAP, "CreatorTool", appNameVersion);
    negXmp->Set(XMP_NS_DC, "format", "image/dng");
    negXmp->SetString(XMP_NS_PHOTOSHOP, "DateCreated", m_negative->GetExif()->fDateTimeOriginal.Encode_ISO_8601());

    m_negative->ResetXMP(negXmp.Release());
}


void NegativeProcessor::backupProprietaryData() {
    AutoPtr<dng_memory_stream> DNGPrivateTag(createDNGPrivateTag());

    if (DNGPrivateTag.Get()) {
        AutoPtr<dng_memory_block> blockPriv(DNGPrivateTag->AsMemoryBlock(m_host->Allocator()));
        m_negative->SetPrivateData(blockPriv);
    }
}


dng_memory_stream* NegativeProcessor::createDNGPrivateTag() {
    uint32 mnOffset = 0;
    dng_string mnByteOrder;
    long mnLength = 0;
    unsigned char* mnBuffer = 0;

    if (getRawExifTag(ExifTag::MakerNote_Offset, 0, &mnOffset) &&
        getRawExifTag(ExifTag::MakerNote_ByteOrder, &mnByteOrder) &&
        getRawExifTag(ExifTag::Photo_MakerNote, &mnLength, &mnBuffer)) {
        bool padding = (mnLength & 0x01) == 0x01;

        dng_memory_stream *streamPriv = new dng_memory_stream(m_host->Allocator());
        streamPriv->SetBigEndian();

        // -----------------------------------------------------------------------------------------
        // Use Adobe vendor-format for encoding MakerNotes in DNGPrivateTag

        streamPriv->Put("Adobe", 5);
        streamPriv->Put_uint8(0x00);

        streamPriv->Put("MakN", 4);
        streamPriv->Put_uint32(mnLength + mnByteOrder.Length() + 4 + (padding ? 1 : 0));
        streamPriv->Put(mnByteOrder.Get(), mnByteOrder.Length());
        streamPriv->Put_uint32(mnOffset);
        streamPriv->Put(mnBuffer, mnLength);
        if (padding) streamPriv->Put_uint8(0x00);

        delete[] mnBuffer;

        return streamPriv;
    }

    return NULL;
}


void NegativeProcessor::buildDNGImage(unsigned short *rawBuffer) {
    //libraw_image_sizes_t *sizes = &m_RawProcessor->imgdata.sizes;

    // -----------------------------------------------------------------------------------------
    // Select right data source from LibRaw

    uint32 outputPlanes = 1;

    // -----------------------------------------------------------------------------------------
    // Create new dng_image and copy data

    //dng_rect bounds = dng_rect(sizes->raw_height, sizes->raw_width);
    dng_rect bounds = dng_rect(image_height, image_width);
    dng_simple_image *image = new dng_simple_image(bounds, outputPlanes, ttShort, m_host->Allocator());

    dng_pixel_buffer buffer; image->GetPixelBuffer(buffer);
    unsigned short *imageBuffer = (unsigned short*)buffer.fData;

    memcpy(imageBuffer, rawBuffer, image_height * image_width * outputPlanes * sizeof(unsigned short));

    AutoPtr<dng_image> castImage(dynamic_cast<dng_image*>(image));
    m_negative->SetStage1Image(castImage);
}


void NegativeProcessor::embedOriginalRaw(const char *rawFilename) {
    #define BLOCKSIZE 65536 // as per spec

    // -----------------------------------------------------------------------------------------
    // Open input/output streams and write header with empty indices

    dng_file_stream rawDataStream(rawFilename);
    rawDataStream.SetReadPosition(0);

    uint32 rawFileSize = static_cast<uint32>(rawDataStream.Length());
    uint32 numberRawBlocks = static_cast<uint32>(floor((rawFileSize + 65535.0) / 65536.0));

    dng_memory_stream embeddedRawStream(m_host->Allocator());
    embeddedRawStream.SetBigEndian(true);
    embeddedRawStream.Put_uint32(rawFileSize);
    for (uint32 block = 0; block < numberRawBlocks; block++) 
        embeddedRawStream.Put_uint32(0);  // indices for the block-offsets
    embeddedRawStream.Put_uint32(0);  // index to next data fork

    uint32 indexOffset = 1 * sizeof(uint32);
    uint32 dataOffset = (numberRawBlocks + 1 + 1) * sizeof(uint32);

    for (uint32 block = 0; block < numberRawBlocks; block++) {

        // -----------------------------------------------------------------------------------------
        // Read and compress one 64k block of data

        z_stream zstrm;
        zstrm.zalloc = Z_NULL;
        zstrm.zfree = Z_NULL;
        zstrm.opaque = Z_NULL;
        if (deflateInit(&zstrm, Z_DEFAULT_COMPRESSION) != Z_OK) 
            throw std::runtime_error("Error initialising ZLib for embedding raw file!");

        unsigned char inBuffer[BLOCKSIZE], outBuffer[BLOCKSIZE * 2];
        uint32 currentRawBlockLength = 
            static_cast<uint32>(std::min(static_cast<uint64>(BLOCKSIZE), rawFileSize - rawDataStream.Position()));
        rawDataStream.Get(inBuffer, currentRawBlockLength);
        zstrm.avail_in = currentRawBlockLength;
        zstrm.next_in = inBuffer;
        zstrm.avail_out = BLOCKSIZE * 2;
        zstrm.next_out = outBuffer;
        if (deflate(&zstrm, Z_FINISH) != Z_STREAM_END)
            throw std::runtime_error("Error compressing chunk for embedding raw file!");

        uint32 compressedBlockLength = zstrm.total_out;
        deflateEnd(&zstrm);

        // -----------------------------------------------------------------------------------------
        // Write index and data

        embeddedRawStream.SetWritePosition(indexOffset);
        embeddedRawStream.Put_uint32(dataOffset);
        indexOffset += sizeof(uint32);

        embeddedRawStream.SetWritePosition(dataOffset);
        embeddedRawStream.Put(outBuffer, compressedBlockLength);
        dataOffset += compressedBlockLength;
    }

    embeddedRawStream.SetWritePosition(indexOffset);
    embeddedRawStream.Put_uint32(dataOffset);

    // -----------------------------------------------------------------------------------------
    // Write 7 "Mac OS forks" as per spec - empty for us

    embeddedRawStream.SetWritePosition(dataOffset);
    embeddedRawStream.Put_uint32(0);
    embeddedRawStream.Put_uint32(0);
    embeddedRawStream.Put_uint32(0);
    embeddedRawStream.Put_uint32(0);
    embeddedRawStream.Put_uint32(0);
    embeddedRawStream.Put_uint32(0);
    embeddedRawStream.Put_uint32(0);

    AutoPtr<dng_memory_block> block(embeddedRawStream.AsMemoryBlock(m_host->Allocator()));
    m_negative->SetOriginalRawFileData(block);
    m_negative->FindOriginalRawFileDigest();
}


// -----------------------------------------------------------------------------------------
// Protected helper functions

bool NegativeProcessor::getInterpretedRawExifTag(const char* exifTagName, int32 component, uint32* value) {
    /*Exiv2::ExifData::const_iterator it = m_RawExif.findKey(Exiv2::ExifKey(exifTagName));
    if (it == m_RawExif.end()) return false;

    std::stringstream interpretedValue; it->write(interpretedValue, &m_RawExif);

    uint32 tmp;
    for (int i = 0; (i <= component) && !interpretedValue.fail(); i++) interpretedValue >> tmp;
    if (interpretedValue.fail()) return false;

    *value = tmp;
    return true;*/
    return false;
}

bool NegativeProcessor::getRawExifTag(ExifTag exifTag, dng_string* value) {
    char * str = exif_bindings.get_string(exif_context, exifTag);
    bool success = (str != nullptr);

    if (success) {
        value->Set_ASCII(str);
        value->TrimLeadingBlanks(); value->TrimTrailingBlanks();
    }

    exif_bindings.free_rs_string(str);
    return success;
}

bool NegativeProcessor::getRawExifTag(ExifTag exifTag, dng_date_time_info* value) {
    char * str = exif_bindings.get_string(exif_context, exifTag);
    bool success = (str != nullptr);

    if (success) {
        dng_date_time dt;
        dt.Parse(str);
        value->SetDateTime(dt);
    }

    exif_bindings.free_rs_string(str);
    return success;
}

bool NegativeProcessor::getRawExifTag(ExifTag exifTag, int32 component, dng_srational* rational) {
    ExifRational ratio;
    auto success = exif_bindings.get_rational(exif_context, exifTag, component, &ratio);

    if (success) { *rational = dng_srational(ratio.numerator, ratio.denominator); return true; }
    return false;
}

bool NegativeProcessor::getRawExifTag(ExifTag exifTag, int32 component, dng_urational* rational) {
    ExifURational ratio;
    auto success = exif_bindings.get_urational(exif_context, exifTag, component, &ratio);

    if (success) { *rational = dng_urational(ratio.numerator, ratio.denominator); return true; }
    return false;
}

bool NegativeProcessor::getRawExifTag(ExifTag exifTag, int32 component, uint32* value) {
    auto success = (bool) exif_bindings.get_uint(exif_context, exifTag, component, value);

    return success;
}

int NegativeProcessor::getRawExifTag(ExifTag exifTag, uint32* valueArray, int32 maxFill) {
    int i = 0;
    for (; i < maxFill; i++) {
        uint32 v;
        auto success = this->getRawExifTag(exifTag, i, &v);
        if (!success) break;

        valueArray[i] = v;
    }
    return i;
}

int NegativeProcessor::getRawExifTag(ExifTag exifTag, int16* valueArray, int32 maxFill) {
    return 0;
}

int NegativeProcessor::getRawExifTag(ExifTag exifTag, dng_urational* valueArray, int32 maxFill) {
    int i = 0;
    for (; i < maxFill; i++) {
        dng_urational v;
        auto success = this->getRawExifTag(exifTag, i, &v);
        if (!success) break;

        valueArray[i] = v;
    }
    return i;
}

bool NegativeProcessor::getRawExifTag(ExifTag exifTag, long* size, unsigned char** data) {
    /*Exiv2::ExifData::const_iterator it = m_RawExif.findKey(Exiv2::ExifKey(exifTagName));
    if (it == m_RawExif.end()) return false;

    *data = new unsigned char[(*it).size()]; *size = (*it).size();
    (*it).copy((Exiv2::byte*)*data, Exiv2::bigEndian);
    return true;*/
    return false;
}
