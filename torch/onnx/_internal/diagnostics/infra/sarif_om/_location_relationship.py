# DO NOT EDIT! This file was generated by jschema_to_python version 0.0.1.dev29,
# with extension for dataclasses and type annotation.

from __future__ import annotations

import dataclasses
from typing import Any


@dataclasses.dataclass
class LocationRelationship(object):
    """Information about the relation of one location to another."""

    target: Any
    description: Any
    kinds: Any
    properties: Any


# flake8: noqa