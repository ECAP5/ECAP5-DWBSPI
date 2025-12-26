Control register (SPI_CR)
""""""""""""""""""""""""""

SPI_CR contains the control for selecting the SPI frequency and controlling the chip select.

.. bitfield::
    :bits: 32
    :lanes: 2
    :vspace: 70
    :hspace: 700

        [
            { "name": "CS", "bits": 1},
            { "name": "reserved", "bits": 15, "type": 1},
            { "name": "PRESCALER", "bits": 16}
        ]

|

.. list-table::
  :header-rows: 1
  :widths: 1 1 99
  
  * - Position
    - Field
    - Description

  * - 31-16
    - Prescaler
    - *SPI frequency selector*

      The specified SPI frequency is f_spi = (f_sys / (2 * prescaler)).
  * - 15-1
    - reserved
    - *This field is reserved.*

      This read-only field is reserved and always has the value 0.
  * - 0
    - CS
    - *Active-high Chip select*

      0 |tab| Chip-select disabled

      1 |tab| Chip-select enabled
