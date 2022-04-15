#!/bin/bash
#set -x
for text in /work/smarherl/matext/dna/dna /work/smarherl/matext/dna.4GiB/dna.4GiB /work/smarherl/matext/dewiki.4GiB/dewiki.4GiB

do
	patterns=${text}.q30
	export text
	export patterns
	./run_task.sh
done

echo "Done"
