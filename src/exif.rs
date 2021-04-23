use std::os::raw::{c_ushort, c_uint};
use num::rational::Ratio;

use crate::bindings::{ExifBindings, ExifRational, ExifURational, ExifTag};
use std::ffi::c_void;


pub trait ExifExtractable {
    fn get_uint(&self, tag: ExifTag, index: u16) -> Option<u32>;
    fn get_urational(&self, tag: ExifTag, index: u16) -> Option<Ratio<u32>>;
    fn get_srational(&self, tag: ExifTag, index: u16) -> Option<Ratio<i32>>;
}


pub struct EmptyExif {}
impl ExifExtractable for EmptyExif {
    fn get_uint(&self, tag: u32, index: u16) -> Option<u32> { None }
    fn get_urational(&self, tag: u32, index: u16) -> Option<Ratio<u32>> { None }
    fn get_srational(&self, tag: u32, index: u16) -> Option<Ratio<i32>> { None }
}


pub struct ExifBox {
    pub extractable: Box<dyn ExifExtractable>,
}

impl ExifBindings {
    pub fn create() -> ExifBindings {
        ExifBindings {
            get_uint: Option::from(exif_get_uint as unsafe extern "C" fn(*mut c_void, u32, u16, *mut c_uint) -> c_uint),
            get_urational: Option::from(exif_get_uratio as unsafe extern "C" fn(*mut c_void, u32, u16, *mut ExifURational) -> c_uint),
            get_rational: Option::from(exif_get_sratio as unsafe extern "C" fn(*mut c_void, u32, u16, *mut ExifRational) -> c_uint)
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