use std::os::raw::{c_ushort, c_uint, c_char};
use num::rational::Ratio;

use crate::bindings::{ExifBindings, ExifRational, ExifURational, ExifTag};
use std::ffi::{c_void, CString};
use chrono::naive::NaiveDateTime;
use super::log_debug;


pub trait ExifExtractable {
    fn get_uint(&self, tag: ExifTag, index: u16) -> Option<u32>;
    fn get_urational(&self, tag: ExifTag, index: u16) -> Option<Ratio<u32>>;
    fn get_srational(&self, tag: ExifTag, index: u16) -> Option<Ratio<i32>>;
    fn get_string(&self, tag: ExifTag) -> Option<String>;
    fn get_datetime(&self, tag: ExifTag) -> Option<NaiveDateTime>;
}


pub struct EmptyExif {}
impl ExifExtractable for EmptyExif {
    fn get_uint(&self, tag: u32, index: u16) -> Option<u32> { None }
    fn get_urational(&self, tag: u32, index: u16) -> Option<Ratio<u32>> { None }
    fn get_srational(&self, tag: u32, index: u16) -> Option<Ratio<i32>> { None }
    fn get_string(&self, tag: u32) -> Option<String> { None }
    fn get_datetime(&self, tag: u32) -> Option<NaiveDateTime> { None }
}


pub struct ExifBox {
    pub extractable: Box<dyn ExifExtractable>,
}

impl ExifBindings {
    pub fn create() -> ExifBindings {
        ExifBindings {
            get_uint: Option::from(exif_get_uint as unsafe extern "C" fn(*mut c_void, u32, u16, *mut c_uint) -> c_uint),
            get_urational: Option::from(exif_get_uratio as unsafe extern "C" fn(*mut c_void, u32, u16, *mut ExifURational) -> c_uint),
            get_rational: Option::from(exif_get_sratio as unsafe extern "C" fn(*mut c_void, u32, u16, *mut ExifRational) -> c_uint),
            get_string: Option::from(exif_get_string as unsafe extern "C" fn(*mut c_void, u32) -> *mut c_char),
            free_rs_string: Option::from(exif_free_string as unsafe extern "C" fn (*mut c_char)),
            log_debug: Option::from(log_debug as unsafe extern "C" fn (*const c_char))
        }
    }
}

unsafe extern "C" fn exif_get_uint(container: *mut c_void, tag: ExifTag, index: c_ushort, value: *mut c_uint) -> c_uint {
    let cn = &mut *(container as *mut ExifBox);
    let res = cn.extractable.get_uint(tag, index);

    match res {
        None => 0,
        Some(x) => {
            *value = x;
            1
        }
    }
}


unsafe extern "C" fn exif_get_uratio(container: *mut c_void, tag: ExifTag, index: c_ushort, value: *mut ExifURational) -> c_uint {
    let cn = &*(container as *mut ExifBox);
    let res = cn.extractable.get_urational(tag, index);

    match res {
        None => 0,
        Some(x) => {
            *value = ExifURational {
                numerator: *x.numer(),
                denominator: *x.denom()
            };
            1
        }
    }
}

unsafe extern "C" fn exif_get_sratio(container: *mut c_void, tag: ExifTag, index: c_ushort, value: *mut ExifRational) -> c_uint {
    let cn = &*(container as *mut ExifBox);
    let res = cn.extractable.get_srational(tag, index);

    match res {
        None => 0,
        Some(x) => {
            *value = ExifRational {
                numerator: *x.numer(),
                denominator: *x.denom()
            };
            1
        }
    }
}

unsafe extern "C" fn exif_get_string(container: *mut c_void, tag: ExifTag) -> *mut c_char {
    let cn = &*(container as *mut ExifBox);
    let res = cn.extractable.get_string(tag);

    match res {
        None => std::ptr::null_mut(),
        Some(x) => match CString::new(x) {
            Ok(x) => x.into_raw(),
            Err(_) => std::ptr::null_mut(),
        }
    }
}

unsafe extern "C" fn exif_free_string(s: *mut c_char) {
    if s.is_null() { return; }
    CString::from_raw(s);
}