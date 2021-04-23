use crate::bindings::{Area, ImageInfoContainer};
use crate::exif::{ExifContainer, DummyExif};

use arrayvec::ArrayVec;
use rawloader::RawImage;
use crate::DNGWriter;


pub trait RawSavableImage {
    fn get_make_model(&self) -> (String, String);
    fn get_info_container(&self) -> ImageInfoContainer;
    fn get_image_data(&self) -> Vec<u16>;
}

pub trait DNGWriting {
    fn get_dng_writer(&self, exif: ExifContainer) -> DNGWriter;
}

impl<T> DNGWriting for T where T: RawSavableImage {
    fn get_dng_writer(&self, exif: ExifContainer) -> DNGWriter {
        let (make, model) = self.get_make_model();
        let writer = DNGWriter::new(self.get_info_container(), self.get_image_data(), exif, make, model);

        writer
    }
}

impl RawSavableImage for RawImage {
    fn get_make_model(&self) -> (String, String) {
        (self.clean_make.clone(), self.clean_model.clone())
    }

    fn get_info_container(&self) -> ImageInfoContainer {
        let black_levels: ArrayVec<_, 4> = self.blacklevels.iter().map(|x| *x as f64).collect();
        let white_levels: ArrayVec<_, 4> = self.whitelevels.iter().map(|x| *x as f64).collect();
        let neutrals: ArrayVec<_, 3> = self.wb_coeffs[0..3].iter().map(|x| 1.0 / (*x as f64)).collect();
        let colormatrix: ArrayVec<_, 3> = self.xyz_to_cam[0..3].iter().map(|x| *x).collect();

        ImageInfoContainer {
            width: self.width as u16,
            height: self.height as u16,
            black_levels: black_levels.into_inner().unwrap(),
            white_levels: white_levels.into_inner().unwrap(),
            camera_neutral: neutrals.into_inner().unwrap(),
            xyz_to_cam: colormatrix.into_inner().unwrap(),
            active_area: Area {
                top: self.crops[0] as u16,
                left: self.crops[3] as u16,
                bottom: (self.height - self.crops[2]) as u16,
                right: (self.width - self.crops[1]) as u16
            },
        }
    }

    fn get_image_data(&self) -> Vec<u16> {
        if let rawloader::RawImageData::Integer(data) = &self.data {
            data.clone()
        } else {
            unimplemented!("Can't parse RAWs with non-integer data, yet.");
        }
    }
}