Memory Map and Registers
========================

.. |tab|   unicode:: U+00A0 .. NO-BREAK SPACE

.. list-table::
  :header-rows: 1
  :widths: 1 94 1 1 1 1
  
  * - Address Offset
    - Register name
    - Width (in bits)
    - Access
    - Reset value
    - Section/page

  * - 0000_0000h
    - Status register (SPI_SR)
    - 32
    - R
    - 0000_0000h
    - :ref:`SPI_SR <GUIDE_SPI_SR>`
  * - 0000_0004h
    - Control register (SPI_CR)
    - 32
    - R/W
    - 0000_0000h
    - :ref:`SPI_CR <GUIDE_SPI_CR>`
  * - 0000_0008h
    - Receive Data register (SPI_RXDR)
    - 32
    - R
    - 0000_0000h
    - :ref:`SPI_RXDR <GUIDE_SPI_RXDR>`
  * - 0000_000Ch
    - Transmit Data register (SPI_TXDR)
    - 32
    - W
    - 0000_0000h
    - :ref:`SPI_TXDR <GUIDE_SPI_TXDR>`

.. _GUIDE_SPI_SR:
.. include:: ../spec/content/spi_sr.rst

.. _GUIDE_SPI_CR:
.. include:: ../spec/content/spi_cr.rst

.. _GUIDE_SPI_RXDR:
.. include:: ../spec/content/spi_rxdr.rst

.. _GUIDE_SPI_TXDR:
.. include:: ../spec/content/spi_txdr.rst

