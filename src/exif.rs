use std::os::raw::{c_ushort, c_uint};
use num::rational::Ratio;

use crate::bindings::{ExifBindings, ExifRational, ExifURational, ExifTag, ExifTag_ExposureTime};
use std::ffi::c_void;


pub trait ExifExtractable {
    fn get_uint(&self, tag: ExifTag, index: u16) -> u32;
    fn get_urational(&self, tag: ExifTag, index: u16) -> Ratio<u32>;
    fn get_srational(&self, tag: ExifTag, index: u16) -> Ratio<i32>;
}


pub struct DummyExif {
    pub dummy: u32
}
impl ExifExtractable for DummyExif {
    fn get_uint(&self, tag: u32, index: u16) -> u32 {
        self.dummy
    }

    fn get_urational(&self, tag: u32, index: u16) -> Ratio<u32> {
        Ratio::new(1, 1)
    }

    fn get_srational(&self, tag: u32, index: u16) -> Ratio<i32> {
        Ratio::new(-1, 1)
    }
}


pub struct ExifContainer {
    pub extractable: Box<dyn ExifExtractable>,
    pub dummy_num: u32
}

impl ExifBindings {
    pub fn create() -> ExifBindings {
        ExifBindings {
            get_uint: Option::from(exif_get_uint as unsafe extern "C" fn(*mut c_void, u32, u16) -> c_uint),
            get_urational: Option::from(exif_get_uratio as unsafe extern "C" fn(*mut c_void, u32, u16) -> ExifURational),
            get_rational: Option::from(exif_get_sratio as unsafe extern "C" fn(*mut c_void, u32, u16) -> ExifRational)
        }
    }
}

unsafe extern "C" fn exif_get_uint(container: *mut c_void, tag: ExifTag, index: c_ushort) -> c_uint {
    print!("Casting...");
    let cn = &mut *(container as *mut ExifContainer);
    print!("Calling...");
    let res = cn.extractable.get_uint(tag, index);
    //let res = cn.extractable.dummy;
    //let res = cn.dummy_num;
    println!("Returning (dummy {})", cn.dummy_num);

    res
}


unsafe extern "C" fn exif_get_uratio(container: *mut c_void, tag: ExifTag, index: c_ushort) -> ExifURational {
    let cn = &*(container as *mut ExifContainer);
    let val = cn.extractable.get_urational(tag, index);

    ExifURational {
        numerator: *val.numer(),
        denominator: *val.denom()
    }
}

unsafe extern "C" fn exif_get_sratio(container: *mut c_void, tag: ExifTag, index: c_ushort) -> ExifRational {
    let cn = &*(container as *mut ExifContainer);
    let val = cn.extractable.get_srational(tag, index);

    ExifRational {
        numerator: *val.numer(),
        denominator: *val.denom()
    }
}