mod raw;

// pub fn get_request_id() -> i64 {
//     unsafe { raw::getCurrentRequestID() }
// }

pub fn get_ask_count() -> i64 {
    unsafe { raw::get_ask_count() }
}

pub fn get_min_count() -> i64 {
    unsafe { raw::get_min_count() }
}

pub fn get_ans_count() -> i64 {
    unsafe { raw::get_ans_count() }
}

// pub fn get_prepare_block_time() -> i64 {
//     unsafe { raw::getPrepareBlockTime() }
// }

// pub fn get_aggregate_block_time() -> i64 {
//     unsafe { raw::getAggregateBlockTime() }
// }

// pub fn get_validator_address(index: i64) -> Vec<u8> {
//     unsafe {
//         let mut data = vec![0u8; 20];
//         assert_eq!(0, raw::readValidatorAddress(index, data.as_mut_ptr()));
//         data
//     }
// }

pub fn get_calldata() -> Vec<u8> {
    unsafe {
        let data_size = raw::get_calldata_size();
        let mut data = vec![0u8; data_size as usize];
        raw::read_calldata(data.as_mut_ptr() as i64, data_size);
        data
    }
}

pub fn save_return_data(data: &[u8]) {
    unsafe { raw::set_return_data(data.as_ptr() as i64, data.len() as i64) }
}

pub fn request_external_data(data_source_id: i64, external_id: i64, calldata: &[u8]) {
    unsafe {
        raw::ask_external_data(
            data_source_id,
            external_id,
            calldata.as_ptr() as i64,
            calldata.len() as i64,
        )
    }
}

pub fn get_external_data(external_id: i64, validator_index: i64) -> Option<String> {
    unsafe {
        let data_size = raw::get_external_data_size(external_id, validator_index);
        if data_size == -1 {
            None
        } else {
            let mut data = vec![0u8; data_size as usize];
            raw::read_external_data(
                external_id,
                validator_index,
                data.as_mut_ptr() as i64,
                data_size,
            );
            Some(String::from_utf8_unchecked(data))
        }
    }
}
