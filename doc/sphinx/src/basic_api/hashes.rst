hash_tables::hashes namespace
==================================

Namespace of hash functions.

Contents
------------------

- Hash functions

  - :cpp:type:`hash_tables::hashes::default_hash`

    - Alias of the default hash function.

  - :cpp:class:`hash_tables::hashes::std_hash`

    - Class to wrap ``std::hash`` class.

- Utility

  - :cpp:func:`void hash_tables::hashes::mix_hash_numbers(std::uint32_t &to, std::uint32_t number)`
  - :cpp:func:`void hash_tables::hashes::mix_hash_numbers(std::uint64_t &to, std::uint64_t number)`

    - Functions to mix hash numbers.

  - :cpp:class:`hash_tables::hashes::hash_cache`

    - Class to cache hash numbers.

Hash functions
---------------------

.. doxygentypedef:: hash_tables::hashes::default_hash

.. doxygenclass:: hash_tables::hashes::std_hash

Utility
--------------

.. doxygenfunction:: hash_tables::hashes::mix_hash_numbers(std::uint32_t &to, std::uint32_t number)

.. doxygenfunction:: hash_tables::hashes::mix_hash_numbers(std::uint64_t &to, std::uint64_t number)

.. doxygenclass:: hash_tables::hashes::hash_cache
