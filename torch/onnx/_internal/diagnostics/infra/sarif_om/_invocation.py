# DO NOT EDIT! This file was generated by jschema_to_python version 0.0.1.dev29,
# with extension for dataclasses and type annotation.

from __future__ import annotations

import dataclasses
from typing import Any


@dataclasses.dataclass
class Invocation(object):
    """The runtime environment of the analysis tool run."""

    execution_successful: Any
    account: Any
    arguments: Any
    command_line: Any
    end_time_utc: Any
    environment_variables: Any
    executable_location: Any
    exit_code: Any
    exit_code_description: Any
    exit_signal_name: Any
    exit_signal_number: Any
    machine: Any
    notification_configuration_overrides: Any
    process_id: Any
    process_start_failure_message: Any
    properties: Any
    response_files: Any
    rule_configuration_overrides: Any
    start_time_utc: Any
    stderr: Any
    stdin: Any
    stdout: Any
    stdout_stderr: Any
    tool_configuration_notifications: Any
    tool_execution_notifications: Any
    working_directory: Any


# flake8: noqa