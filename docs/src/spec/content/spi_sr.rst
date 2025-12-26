Status register (SPI_SR)
"""""""""""""""""""""""""

.. bitfield::
    :bits: 32
    :lanes: 2
    :vspace: 70
    :hspace: 700

        [
            { "name": "TXE", "bits": 1},
            { "name": "reserved", "bits": 31, "type": 1}
        ]

|

.. list-table::
  :header-rows: 1
  :widths: 1 1 99
  
  * - Position
    - Field
    - Description

  * - 31-1
    - Reserved
    - *This field is reserved.*

      This read-only field is reserved and always has the value 0.
  * - 0
    - TXE
    - *Transmit register Empty*

      0 |tab| The transmit register is full (being sent)

      1 |tab| The transmit register is empty
