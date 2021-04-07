#![feature(vec_into_raw_parts)]

use std::ffi::CString;
use std::ffi::c_void;
use std::os::raw::c_char;
use std::os::raw::c_ushort;
use std::path::Path;

#[macro_use]
extern crate version;

extern "C" {
    fn createConverter(app_name: *const c_char, app_version: *const c_char, width: u16, height: u16) -> *const c_void;
    fn destroyConverter(handler: *const c_void);
    fn callDummy(handler: *const c_void);
    fn buildNegative(handler: *const c_void, image_buffer: *mut c_ushort);
    fn writeDNG(handler: *const c_void, path: *const c_char);
    fn writeTIFF(handler: *const c_void, path: *const c_char);
    fn writeJPEG(handler: *const c_void, path: *const c_char);
}


pub struct DNGWriter {
    handler: *const c_void
}


impl DNGWriter {
    pub fn new(width: u16, height: u16) -> DNGWriter {
        let app_str = CString::new("libdng-rs").unwrap();
        let ver_str = CString::new(version!()).unwrap();
        unsafe {
            DNGWriter {
                handler: createConverter(app_str.as_ptr(), ver_str.as_ptr() , width, height)
            }
        }
    }

    pub fn dummy(&self) {
        unsafe {
            callDummy(self.handler.as_ref().unwrap());
        }
    }

    pub fn build_negative(&self, image_data: Vec<u16>) {
        let (ptr, len, _cap) = image_data.into_raw_parts();
        println!("Creating negative of size {}", len);

        unsafe {
            buildNegative(self.handler, ptr);
        }
    }

    pub fn write_dng(&self, output: &Path) {
        println!("Writing DNG to '{}'...", output.display());
        let out_str = CString::new(output.as_os_str().to_str().unwrap()).unwrap();

        unsafe {
            writeDNG(self.handler, out_str.as_ptr())
        }
    }

    pub fn write_tif(&self, output: &Path) {
        println!("Writing TIFF to '{}'...", output.display());
        let out_str = CString::new(output.as_os_str().to_str().unwrap()).unwrap();

        unsafe {
            writeTIFF(self.handler, out_str.as_ptr())
        }
    }

    pub fn write_jpg(&self, output: &Path) {
        println!("Writing JPEG to '{}'...", output.display());
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


#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_handler_creation() {
        let dng_handler = DNGWriter::new(200, 100);
        dng_handler.dummy();
    }
}
