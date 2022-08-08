#![allow(non_upper_case_globals)]

pub mod image_info;
pub mod bindings;
pub mod exif;

use std::ffi::{c_void, CString, CStr};
use std::os::raw::c_char;
use std::os::raw::c_ushort;
use std::path::Path;
use bindings::ImageInfoContainer;
use crate::exif::ExifBox;
use log::{debug, error, info};


extern "C" {
    fn createConverter(image: ImageInfoContainer, image_buffer: *mut c_ushort, exif_bindings: bindings::ExifBindings, exif_context: *const c_void, make: *const c_char, model: *const c_char) -> *const c_void;
    fn destroyConverter(handler: *const c_void);
    fn callDummy(handler: *const c_void);
    fn setAppName(handler: *const c_void, app_name: *const c_char, app_version: *const c_char);
    fn writeDNG(handler: *const c_void, path: *const c_char);
    fn writeTIFF(handler: *const c_void, path: *const c_char);
    fn writeJPEG(handler: *const c_void, path: *const c_char);
}


pub struct DNGWriter {
    handler: *const c_void,
}


impl DNGWriter {
    pub fn new(info: ImageInfoContainer, image_data: Vec<u16>, mut exif: ExifBox, make: String, model: String) -> DNGWriter {
        let make_str = CString::new(make).unwrap();
        let model_str = CString::new(model).unwrap();
        let mut data = image_data.into_boxed_slice();

        unsafe {
            DNGWriter {
                handler: createConverter( info,
                                          data.as_mut_ptr(),
                                          bindings::ExifBindings::create(),
                                          &mut exif as *mut exif::ExifBox as *mut c_void,
                                          make_str.as_ptr(),
                                          model_str.as_ptr()
                )
            }
        }
    }

    pub fn dummy(&self) {
        unsafe {
            callDummy(self.handler.as_ref().unwrap());
        }
    }

    pub fn set_app_name(&self, name: String, version: String) {
        let app_str = CString::new(name).unwrap();
        let ver_str = CString::new(version).unwrap();

        unsafe {
            setAppName(self.handler, app_str.as_ptr(), ver_str.as_ptr())
        }
    }

    pub fn write_dng(&self, output: &Path) {
        info!("Writing DNG to '{}'...", output.display());
        let out_str = CString::new(output.as_os_str().to_str().unwrap()).unwrap();

        unsafe {
            writeDNG(self.handler, out_str.as_ptr())
        }
    }

    pub fn write_tif(&self, output: &Path) {
        info!("Writing TIFF to '{}'...", output.display());
        let out_str = CString::new(output.as_os_str().to_str().unwrap()).unwrap();

        unsafe {
            writeTIFF(self.handler, out_str.as_ptr())
        }
    }

    pub fn write_jpg(&self, output: &Path) {
        info!("Writing JPEG to '{}'...", output.display());
        let out_str = CString::new(output.as_os_str().to_str().unwrap()).unwrap();

        unsafe {
            writeJPEG(self.handler, out_str.as_ptr())
        }
    }
}


impl Drop for DNGWriter {
    fn drop(&mut self) {
        unsafe {
            destroyConverter(self.handler.as_ref().unwrap());
        }
    }
}


unsafe extern "C" fn log_debug(s: *const c_char) {
    if s.is_null() { return; }
    let msg = CStr::from_ptr(s);

    match msg.to_str() {
        Ok(x) => debug!("{}", x),
        Err(_) => error!("Can't parse CString"),
    }
}


#[cfg(test)]
mod tests {
    use super::*;

    /*#[test]
    fn test_handler_creation() {
        let dng_handler = DNGWriter::new(200, 100);
        dng_handler.dummy();
    }*/
}
