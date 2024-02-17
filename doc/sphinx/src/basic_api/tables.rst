hash_tables::tables namespace
======================================

Namespace of hash tables.

Contents
------------

- Hash tables for single thread (not thread-safe)

  - :cpp:class:`hash_tables::tables::open_address_table_st`

    - Class of hash tables using open addressing.
    - This is currently fastest among hash tables in this library.

  - :cpp:class:`hash_tables::tables::multi_open_address_table_st`

    - Class of hash tables made of multiple hash tables using open addressing.

- Hash tables for multiple threads (thread-safe)

  - :cpp:class:`hash_tables::tables::multi_open_address_table_mt`

    - Class of concurrent hash tables made of multiple hash tables using open addressing.
    - This is currently fastest among concurrent hash tables in this library.

  - :cpp:class:`hash_tables::tables::separate_shared_chain_table_mt`

    - Class of concurrent hash tables using separate chains.

Reference
----------------------------------

.. doxygenclass:: hash_tables::tables::open_address_table_st

.. doxygenclass:: hash_tables::tables::multi_open_address_table_st

.. doxygenclass:: hash_tables::tables::multi_open_address_table_mt

.. doxygenclass:: hash_tables::tables::separate_shared_chain_table_mt
