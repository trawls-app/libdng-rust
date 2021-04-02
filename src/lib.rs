use std::ffi::c_void;

extern "C" {
    fn getHandler() -> *const c_void;
    fn destroyHandler(handler: *const c_void);
    fn callHandlerDummy(handler: *const c_void);
}

pub struct DNGHandler {
    handler: *const c_void
}


impl DNGHandler {
    pub fn create() -> DNGHandler {
        unsafe {
            DNGHandler { handler: getHandler() }
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
        let dng_handler = DNGHandler::create();
        dng_handler.dummy();
    }
}
