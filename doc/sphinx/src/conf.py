# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options. For a full
# list see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#
# import os
# import sys
# sys.path.insert(0, os.path.abspath('.'))

# -- Project information -----------------------------------------------------

project = "cpp-hash-tables"
release = "unknown"  # inserted in command line

copyright = "2022, MusicScience37 (Kenta Kabashima). "
author = "MusicScience37"

# -- General configuration ---------------------------------------------------

extensions = [
    "sphinx.ext.todo",
    "myst_parser",
]

templates_path = ["_templates"]

exclude_patterns = []

# -- Options for PlantUML ----------------------------------------------------

extensions += ["sphinxcontrib.plantuml"]

plantuml_output_format = "svg"
plantuml_syntax_error_image = True

# -- Options for mathjax -----------------------------------------------------

extensions += ["sphinx.ext.mathjax"]
# force to use MathJax 2 for compatibility with Plotly
mathjax_path = (
    "https://cdn.jsdelivr.net/npm/mathjax@2/MathJax.js?config=TeX-AMS-MML_HTMLorMML"
)
mathjax_config = {
    "TeX": {
        "Macros": {
            "bm": ["{\\boldsymbol{#1}}", 1],
        },
    },
}

# -- Options for breathe -----------------------------------------------------

extensions += ["breathe"]

# this variable must be set from command line
breathe_projects = {"hash_tables": ""}

breathe_default_project = "hash_tables"

breathe_default_members = ("members",)

breathe_domain_by_extension = {
    "h": "cpp",
}

# -- Options for HTML output -------------------------------------------------

html_theme = "sphinx_rtd_theme"
html_static_path = ["_static"]
html_theme_options = {
    "style_nav_header_background": "#B24700",
}
