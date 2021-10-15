import subprocess


def run_application(run_script_path, input_dir, observation_id, start_time, inv_polyphase_filter_path, output_dir,
        ignore_errors) -> subprocess.CompletedProcess:
    """Runs the application with the given arguments."""

    args = [str(run_script_path), str(input_dir), str(observation_id), str(start_time), str(inv_polyphase_filter_path),
        str(output_dir), str(ignore_errors)]
    return subprocess.run(args)
