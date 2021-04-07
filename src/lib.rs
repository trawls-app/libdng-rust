use std::ffi::CString;
use std::ffi::c_void;
use std::os::raw::c_char;

#[macro_use]
extern crate version;

extern "C" {
    fn createConverter(app_name: *const c_char, app_version: *const c_char) -> *const c_void;
    fn destroyConverter(handler: *const c_void);
    fn callDummy(handler: *const c_void);
}


pub struct DNGWriter {
    handler: *const c_void
}


impl DNGWriter {
    pub fn new() -> DNGWriter {
        let app_str = CString::new("libdng-rs").unwrap();
        let ver_str = CString::new(version!()).unwrap();
        unsafe {
            DNGWriter {
                handler: createConverter(app_str.as_ptr(), ver_str.as_ptr() )
            }
        }
    }

    pub fn dummy(&self) {
        unsafe {
            callDummy(self.handler.as_ref().unwrap());
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
        let dng_handler = DNGWriter::new();
        dng_handler.dummy();
    }
}
