use std::ffi::CString;
use std::ffi::c_void;
use std::os::raw::c_char;

#[macro_use]
extern crate version;

extern "C" {
    fn createHandler(app_name: *const c_char, app_version: *const c_char) -> *const c_void;
    fn destroyHandler(handler: *const c_void);
    fn callHandlerDummy(handler: *const c_void);
}

pub struct DNGHandler {
    handler: *const c_void
}


impl DNGHandler {
    pub fn new() -> DNGHandler {
        let app_str = CString::new("libdng-rs").unwrap();
        let ver_str = CString::new(version!()).unwrap();
        unsafe {
            DNGHandler {
                handler: createHandler(app_str.as_ptr(), ver_str.as_ptr() )
            }
        }
    }

    pub fn dummy(&self) {
        unsafe {
            callHandlerDummy(self.handler.as_ref().unwrap());
        }
    }
}

impl Drop for DNGHandler {
    fn drop(&mut self) {
        unsafe {
            destroyHandler(self.handler.as_ref().unwrap());
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_handler_creation() {
        let dng_handler = DNGHandler::new();
        dng_handler.dummy();
    }
}
