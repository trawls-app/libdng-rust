use crate::bindings::ImageInfoContainer;

use arrayvec::ArrayVec;
use rawloader::RawImage;
use std::ffi::CString;


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

    pub fn get_container(&self) -> ImageInfoContainer {
        let black_levels: ArrayVec<_, 4> = self.image.blacklevels.iter().map(|x| *x as f64).collect();
        let white_levels: ArrayVec<_, 4> = self.image.whitelevels.iter().map(|x| *x as f64).collect();
        let neutrals: ArrayVec<_, 3> = self.image.wb_coeffs[0..3].iter().map(|x| 1.0 / (*x as f64)).collect();
        let colormatrix: ArrayVec<_, 3> = self.image.xyz_to_cam[0..3].iter().map(|x| *x).collect();

        ImageInfoContainer {
            make: self.make.as_ptr(),
            model: self.model.as_ptr(),
            width: self.image.width as u16,
            height: self.image.height as u16,
            black_levels: black_levels.into_inner().unwrap(),
            white_levels: white_levels.into_inner().unwrap(),
            camera_neutral: neutrals.into_inner().unwrap(),
            xyz_to_cam: colormatrix.into_inner().unwrap()
        }
    }
}