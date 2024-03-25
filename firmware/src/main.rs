#![no_std]
#![no_main]
//#![feature(type_alias_impl_trait)]

mod onewire;

use defmt::*;
use embassy_executor::Spawner;
use embassy_futures::join::join;
use embassy_stm32::flash::Flash;
use embassy_stm32::gpio::{Level, Output, Speed};
use embassy_stm32::{bind_interrupts, peripherals, Config};
use embassy_time::{Duration, Timer};
use embassy_usb::class::cdc_acm::{CdcAcmClass, State};
use embassy_usb::driver::EndpointError;
use embassy_usb::Builder;
use embedded_hal::digital::OutputPin;
use onewire::commands::*;
use {defmt_rtt as _, panic_probe as _};

#[embassy_executor::main]
async fn main(_spawner: Spawner) {
    let p = embassy_stm32::init(Config::default());

    let mut led = Output::new(p.PB7, Level::High, Speed::Low);

    loop {
        info!("high");
        led.set_high();
        Timer::after_millis(300).await;

        info!("low");
        led.set_low();
        Timer::after_millis(300).await;
    }

    // Future steps will include initializing I2C peripherals and integrating sensor reads
}
