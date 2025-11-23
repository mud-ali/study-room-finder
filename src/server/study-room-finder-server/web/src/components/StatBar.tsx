"use client";

import * as React from "react";
import { Progress } from "@/components/ui/progress";

interface StatBarProps {
    descriptorLabel: string;
    value: number;
    roomName: string;
}

function getProgressBarColors(descriptor: string) {
    switch (descriptor) {
        case "Low":
            return "[&>div]:bg-green-400";
        case "Medium":
            return "[&>div]:bg-yellow-400";
        case "High":
            return "[&>div]:bg-red-300";
        default:
            return "";
    }
}

export default function StatBar({ descriptorLabel, value, roomName }: StatBarProps) {
    const [progress, setProgress] = React.useState(13);

    const getDescriptorFromPercent = (percent: number) => {
        if (percent < 40) return "Low";
        else if (percent < 70) return "Medium";
        else return "High";
    };

    let descriptor = getDescriptorFromPercent(value ?? 0);

    React.useEffect(() => {
        const timer = setTimeout(() => setProgress(66), 500);
        return () => clearTimeout(timer);
    }, []);

    // use descriptorLabel to create a meaningful id (e.g. "volume-bar-roomA")
    const id = `${descriptorLabel.toLowerCase()}-bar-${roomName}`;

    return (
        <>
            <div className="flex items-center justify-between">
                <span>
                    {descriptorLabel}: {descriptor}
                </span>
                <span>~{Math.round(value ?? 0)}%</span>
            </div>
            <Progress id={id} value={value} className={`w-full mt-3 mb-7 ${getProgressBarColors(descriptor)}`} />
        </>
    );
}
