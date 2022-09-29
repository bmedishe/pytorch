# DO NOT EDIT! This file was generated by jschema_to_python version 0.0.1.dev29,
# with extension for dataclasses and type annotation.

from __future__ import annotations

import dataclasses
from typing import Any


@dataclasses.dataclass
class GraphTraversal(object):
    """Represents a path through a graph."""

    description: Any
    edge_traversals: Any
    immutable_state: Any
    initial_state: Any
    properties: Any
    result_graph_index: Any
    run_graph_index: Any


# flake8: noqa