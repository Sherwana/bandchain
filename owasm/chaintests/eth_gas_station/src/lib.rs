use obi::{OBIDecode, OBIEncode};
use owasm::{execute_entry_point, ext, oei, prepare_entry_point};

#[derive(OBIDecode)]
struct Input {
    gas_option: String,
}

#[derive(OBIEncode)]
struct Output {
    gweix10: u64,
}

#[no_mangle]
fn prepare_impl(input: Input) {
    // ETH gas station data source
    oei::request_external_data(11, 1, &input.gas_option.as_bytes());
}

#[no_mangle]
fn execute_impl(_: Input) -> Output {
    let avg: f64 = ext::load_average(1);
    Output { gweix10: avg as u64 }
}

prepare_entry_point!(prepare_impl);
execute_entry_point!(execute_impl);
