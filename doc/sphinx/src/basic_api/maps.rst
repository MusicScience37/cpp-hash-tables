hash_tables::maps namespace
======================================

Namespace of maps from keys to values.

Contents
------------

- Maps for single thread (not thread-safe)

  - :cpp:class:`hash_tables::maps::open_address_map_st`

    - Class of maps using open addressing.
    - This is currently fastest among maps in this library.

  - :cpp:class:`hash_tables::maps::multi_open_address_map_st`

    - Class of maps made of multiple hash tables using open addressing.

- Maps for multiple thread (thread-safe)

  - :cpp:class:`hash_tables::maps::multi_open_address_map_mt`

    - Class of concurrent maps made of multiple hash tables using open addressing.
    - This is currently fastest among concurrent maps in this library.

  - :cpp:class:`hash_tables::maps::separate_shared_chain_map_mt`

    - Class of concurrent maps using separate chains.

Reference
----------------------------------

.. doxygenclass:: hash_tables::maps::open_address_map_st

.. doxygenclass:: hash_tables::maps::multi_open_address_map_st

.. doxygenclass:: hash_tables::maps::multi_open_address_map_mt

.. doxygenclass:: hash_tables::maps::separate_shared_chain_map_mt
