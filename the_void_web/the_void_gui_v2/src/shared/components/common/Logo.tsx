import LogoIcono from '@/shared/assets/logo.svg'

interface LogoProps {
    css?: string;
}

export default function Logo({
    css = ""
}: LogoProps) {
    return (
        <LogoIcono className={css} />
    );
}