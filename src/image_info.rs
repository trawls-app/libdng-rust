use crate::bindings::{Area, ImageInfoContainer};

use arrayvec::ArrayVec;
use rawloader::RawImage;
use std::ffi::CString;
use crate::DNGWriter;


pub trait RawSavableImage {
    fn get_make_model(&self) -> (String, String);
    fn get_info_container(&self) -> ImageInfoContainer;
    fn get_image_data(&self) -> Vec<u16>;
}

pub trait DNGWriting {
    fn get_dng_writer(&self) -> DNGWriter;
}

impl<T> DNGWriting for T where T: RawSavableImage {
    fn get_dng_writer(&self) -> DNGWriter {
        let (make, model) = self.get_make_model();
        let writer = DNGWriter::new(self.get_info_container(), make, model);
        writer.build_negative(self.get_image_data());

        writer
    }
}

pub struct ImageInfo {
    image: RawImage,
    make: CString,
    model: CString,
}

impl ImageInfo {
    pub fn new(image: RawImage) -> Result<ImageInfo, String> {
        if image.cfa.name != "RGGB" {
            return Err(format!("Only RGGB images are currently supported. Got a '{}' image.", image.cfa.name));
        }

        let make = CString::new(image.clean_make.clone()).unwrap();
        let model = CString::new(image.clean_model.clone()).unwrap();

        Ok(ImageInfo { image, make, model })
    }
}

impl RawSavableImage for ImageInfo {
    fn get_make_model(&self) -> (String, String) {
        (self.image.clean_make.clone(), self.image.clean_model.clone())
    }

    fn get_info_container(&self) -> ImageInfoContainer {
        let black_levels: ArrayVec<_, 4> = self.image.blacklevels.iter().map(|x| *x as f64).collect();
        let white_levels: ArrayVec<_, 4> = self.image.whitelevels.iter().map(|x| *x as f64).collect();
        let neutrals: ArrayVec<_, 3> = self.image.wb_coeffs[0..3].iter().map(|x| 1.0 / (*x as f64)).collect();
        let colormatrix: ArrayVec<_, 3> = self.image.xyz_to_cam[0..3].iter().map(|x| *x).collect();

        ImageInfoContainer {
            width: self.image.width as u16,
            height: self.image.height as u16,
            black_levels: black_levels.into_inner().unwrap(),
            white_levels: white_levels.into_inner().unwrap(),
            camera_neutral: neutrals.into_inner().unwrap(),
            xyz_to_cam: colormatrix.into_inner().unwrap(),
            active_area: Area {
                top: self.image.crops[0] as u16,
                left: self.image.crops[3] as u16,
                bottom: (self.image.height - self.image.crops[2]) as u16,
                right: (self.image.width - self.image.crops[1]) as u16
            },
        }
    }

    fn get_image_data(&self) -> Vec<u16> {
        if let rawloader::RawImageData::Integer(data) = &self.image.data {
            data.clone()
        } else {
            unimplemented!("Can't parse RAWs with non-integer data, yet.");
        }
    }
}